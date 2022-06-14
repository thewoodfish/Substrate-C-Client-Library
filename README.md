# Samaritan
A possible solution to decentralized identity

#### Please note that the below is just imaginations and is subject to quick change, as soon as the next minute. You're free to contribute in every way :)

The idea is to build a layer beneath the user space but above the very kernel of an OS, that can communicate with a Substrate chain and other decentralized infrastructures e.g for storage.

On the Samaritan OS, a user is asked to fill in personal details with well-defined fields that do not change. This goes to a decentralized storage and is recorded as a transaction on a substrate chain.
When an app is created, the app creator signs the app onchain and uploads it to decentralized storage with its CID stored onchain. On app creation, the details needed by the apps would be well defined and stated already. The app creator stakes a particular amount, and pays validators to validate its app, before being added to the app pool. So after validation, if successful, the app is approved for download and added to the pool. Another transaction. If not, the money staked is returned but the fees are not. The app can be removed also from the pool, if its voted out by 2/3 of the downloaders. When this happends the staked crypto is slashed, but in the absence of this, the app creator can earn staking rewards through defi. So on download of the app into his Samaritan OS, the user decides to open the app.
On opening the app, the user is explicitly asked by the OS whether to allow or give the apps certain access to data stored in its state. If yes, on entering the app, the user public ID is given to the app, and the data also accompanies it. Now the user should be able to revoke access to that data by submitting a transaction or the access may expire due to the expiry of the lease period. I don't know how this can be implemented yet. Maybe something should be like a place in memory where this data can be read and/or written by the program/app. This data can be committed online, That is why a Samaritan is basically a state. Files not committed online cannot be regarded as part of a Samaritan state. Local apps data are part of a state, but can be flagged as not. 

A samaritan OS hopes to run on a mobile device. A samaritan helps users to become somewhat independent of a device. Its as simple has getting a new device. Inputing your keys and getting your state restored on that device. With your apps too, becuase apps have a presence onchain, which can be traced and installed on the new device along with the local data.
Communication is also an inportant issue with Samaritans.
...



```code```
