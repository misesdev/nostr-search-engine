import { RelayPool } from "../modules/RelayPool";
import { FileSystem } from "../filesytem/disk";
import { Event } from "../modules/types";

const defaultProfile = "https://blob.nostroogle.org/files/storage/ec362271f59dbc624ae0c9654/hczhqsKU5okwFDpeASqhNKwYykBGP1ne1QvtGGCR.png";

const sanitiseUser = (event: Event): any => {

    let user = JSON.parse(event.content)

    let properties = ["name", "displayName", "profile", "about", "pubkey"]

    if((!user["name"] && !user["display_name"]) || user["deleted"]) 
        throw new Error("invalid user")

    if(!user["name"] && user["display_name"])
        user["name"] = user["display_name"]

    if(!user["display_name"] && user["name"])
        user["name"] = user["display_name"]


    if(!user['displayName']) 
        user["displayName"] = user["display_name"]

    if(!user["profile"] && user["picture"]) 
        user["profile"] = user["picture"]

    if(!user["picture"]) 
        user["profile"] = defaultProfile

    if(!user["about"])
        user["about"] = ""

    if(user["about"] && user["about"].length > 100)
        user["about"] = `${user["about"].substring(0, 96)}...`

    user["pubkey"] = event.pubkey

    for(let property in user) {
        if(!properties.includes(property))
            delete user[property]
    }

    return user;
}

export const listUsers = async (pool: RelayPool) => {
    
    const pubkeys: string[] = []

    const fileUsers = new FileSystem("users.db");
    const filePubkeys = new FileSystem("pubkeys.db");

    await filePubkeys.readLines(async (line) => { 
        pubkeys.push(line) 
        return true;
    })

    let skipe = 200, totalUsers = 0
    for (let i = 0; i <= pubkeys.length; i += skipe) 
    {
        let authors = pubkeys.slice(i, i + skipe)

        let events = await pool.fechEvents({
            authors: authors,
            limit: skipe,
            kinds: [0]
        })

        console.log("events:", events.length);

        events.forEach(event => {
            try {
                let user = sanitiseUser(event)

                fileUsers.writeLine(JSON.stringify(user))

                totalUsers++;
            } catch {}
        });
    }

    console.log("found users:", totalUsers)
}
