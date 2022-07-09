# Samaritan
A possible solution to decentralized identity

A samaritan is a digital state of an individual that is very decentralized and 100% owned and access-controlled by the owner. A samaritan can "possess" devices with its state and then allow programs can access that data, in a well defined infrastructure. Ofcourse, there's more to it...

The idea implemented will be to build a layer beneath the user space but above the very kernel of an OS, that can communicate with a Substrate chain and other decentralized infrastructures e.g for storage.

On the Samaritan OS, a user is asked to fill in personal details with well-defined fields that do not change. This goes to a decentralized storage and is recorded as a transaction on a substrate chain.
When an app is created, the app creator signs the app onchain and uploads it to decentralized storage with its CID stored onchain. On app creation, the details needed by the apps would be well defined and stated already. The app creator stakes a particular amount, and pays validators to validate its app, before being added to the app pool. So after validation, if successful, the app is approved for download and added to the pool. Another transaction. If not, the money staked is returned but the fees are not. The app can be removed also from the pool, if its voted out by 2/3 of the downloaders. When this happends the staked crypto is slashed, but in the absence of this, the app creator can earn staking rewards. So on download of the app into his Samaritan OS, the user decides to open the app.
On opening the app, the user is explicitly asked by the OS whether to allow or give the apps certain access to data stored in its state. If yes, on entering the app, the user public ID is given to the app, and the data also accompanies it. Now the user should be able to revoke access to that data by submitting a transaction or the access may expire due to the expiry of the lease period. I don't know how this can be implemented yet. Maybe something slike a place in memory where this data can be read and/or written by the program/app. This data will be committed online, That is why a Samaritan is basically a state. Files not committed online cannot be regarded as part of a Samaritan state. Local apps data are part of a state, but can be flagged as not. 

A samaritan OS hopes to run on a mobile device. A samaritan helps users to become somewhat independent of a device. Its as simple has getting a new device. Inputing your keys and getting your state restored on that device. With your apps too, becuase apps have a presence onchain, which can be traced and installed on the new device along with the local data.
Communication is also an inportant issue with Samaritans.
...


### Prototype development steps
1. A substrate client written in C to communicate to a substrate chain.
2. A minimal chain has to be built containing something like a ```Samaritan struct``` with ```CID```s, ```balance``` etc, a custom pallet and necessary pallets that would make it work basically. Also a couple of storage items need to be created also etc.
3. Samaritan would be experimented on the Minix OS. So after the above, minix internals would be modified to authenticate with a chain, upload data to IPFS and pin on other decentralized storage providers, download and propagate state etc.
4. A simple SDK/protocol that apps use to get access to data from a Samaritan.
5. The a simple program can be built to test the whole network cycle and protocols, from user account creation to app creation to the final stage of app download and access of another user state by the app.

### Progress now
Currently working on step 1. 
 ```main/substrate_interface.c``` contains the main code for the client and is still under dev. <br>
 
### IMPORTANT
This repo has been temporarily abandoned for https://github.com/thewoodfish/sam. Which is a test web version. This is due to the difficulty and time expended in decoding a chains data and other primitive stuff. I don't want to get derailed from the vision battling low-level stuffs. As soon as the web mvp is available, this repo will come to life!

Thank you!
