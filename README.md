# dCache
**by Chloe Scaramal**

### distributed cache system for in-memory storage of key-value objects.

This project is being developed in the course of Distributed Systems of the Information Systems Bachelor's Degree of Federal University of Uberlândia, under the orientation of Rivalino Matias Junior.

It was written in it's entirety with C programming language and help from make.

I have used a client-server architecture in which the server maintains a LFU cache for the client to access objects in a fast and efficient way.

The communication is made using a text protocol developed by me for this specific application, which contain a library for handling it.

A TCP socket connection is used for message exchange from client to server and vise-versa.

The server stores data related to the user in an array by hashing it's identifier, yielding an extremely low complexity access to the requested object.

This project is made for educational purposes only.
