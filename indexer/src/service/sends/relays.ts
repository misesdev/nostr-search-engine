import { FileSystem } from "../../filesytem/disk"

export const sendRelays = async (fileRelays: FileSystem) => {
    
    await fileRelays.readLines(async (line: string) => {
        try 
        {
            let response = await fetch(`http://localhost:8080/add_relay`, {
                method: "post",
                body: JSON.stringify({
                    relay: line.trim()
                })
            }) 

            let data = await response.json()

            console.log("send relay:", line.trim())
            console.log("response ->", data.message)
        }
        catch { return true }
        
        return true
    }) 

    let response = await fetch(`http://localhost:8080/save`, {
        method: "post",
        body: JSON.stringify({
            scope: "relays"
        })
    }) 

    console.log("request save data")

    let data = await response.json()

    console.log("response ->", data.message);
}