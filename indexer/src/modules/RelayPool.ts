import { WebSocket } from "ws"
import { Event, Filter } from "./types"
import { distinctEvent } from "../utils";

export class RelayPool {

    private relays: string[];
    public websockets: WebSocket[];
    public timeout: number = 1000;
    private subscription: string = "3da9794398579582309458d6f1498";

    constructor(relays: string[]) {
        if(relays.length < 1)
            throw Error("expected relays");

        this.relays = relays; 
        this.websockets = [];
    }

    private async connectRelay(relay: string) : Promise<WebSocket> 
    {
        return new Promise((resolve, reject) => {
            let websock = new WebSocket(relay);
            websock.on("open", () => resolve(websock));
            websock.on("close", () => reject(`not connetd: ${relay}`))
            websock.on("error", () => reject(`not connetd: ${relay}`))

            setTimeout(() => {
                //websock.removeAllListeners("open");
                resolve(null)
            }, this.timeout)
        });
    }

    public async connect() 
    {
        console.log("connecting")

        let websockets = this.relays.map(relay => this.connectRelay(relay).catch(error => {
            console.log(error)
            return null;
        }))

        this.websockets = await Promise.all(websockets)

        this.websockets = this.websockets.filter(socket => socket != null)

        console.log("connected")
    }

    private async fetchEventRelay(websocket: WebSocket, filter: Filter): Promise<Event[]> 
    {
        return new Promise((resolve, reject) => {
            let events: Event[] = []
            // send the message
            websocket.send(`[
                "REQ", 
                "${this.subscription}", 
                ${JSON.stringify(filter)}
            ]`);
            
            // receive the event and return
            const handleMessage = (message: any) => {
                let data = JSON.parse(message.toString());            
                
                if(data[0] == "EVENT") {
                    let event: Event = data[2];
                    events.push(event);
                }
                
                if(data[0] == "EOSE") {
                    resolve(events)
                    websocket.removeListener("message", handleMessage)
                }
            }

            websocket.on("message", handleMessage)

            // remove the listener in timeout
            setTimeout(() => { 
                reject(`timeout: ${websocket.url}`)
                websocket.removeAllListeners("message")
            }, this.timeout);
        });
    }

    public async fechEvents(filter: Filter): Promise<Event[]> 
    {
        let eventPromises = this.websockets.map(async (websocket) => { 
            return this.fetchEventRelay(websocket, filter).catch((error:string) => {
                console.log(error)
                return [];
            })
        })

        let allEvents = await Promise.all(eventPromises)

        let events = allEvents.flat()

        return distinctEvent(events)
    }

    public async fechUser(pubkey: string): Promise<Event> 
    {
        let events = await this.fechEvents({
            kinds: [0],
            authors: [pubkey],
            limit: 1
        });

        if(events.length > 0) return events[0];

        return null;
    }
}