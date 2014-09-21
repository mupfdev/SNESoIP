# Protcol specification #



## HELO ##
Init server usage.
### Usage ###
```
HELO server
```
### Answer ###
```
HELO cloent:client-IP
example:
HELO client:10.0.0.2
```



## USER ##
Login with username. On success: OK + iv vector

### Usage ###
```
USER username
```
#### Success answer ####
```
OK
edj93489hef9sw384ht9so84t4s4
```
#### Denied answer ####
```
WRONG
```



## PASS ##
Login with trivium crypted password in a hexstring as ascii representation

### Usage ###
```
PASS 7f8f33deadbeef
```
#### Success answer ####
```
OK
```
#### denied answer ####
```
WRONG
```



## PORT ##
Ask server if UDP port is available. If not, client increments the PORT
command parameter until it's accepted.

### Usage ###
```
PORT protnumber
```
#### Already used under the same ip/nat answer ####
```
USED
```
#### Accepted and opponent has alredy connected answer ####
```
SLAVE
10.0.0.3:4455
```
#### Accepted and you are master answer ####
```
MASTER
```



## QUIT ##
Eends connection if udp connection has established.

### Usage ###
```
QUIT
```
#### Success answer ####
None -> just closes connection.
