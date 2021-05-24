# Building
In  Order  to  build  the  containers,  make  sure  docker  is  installed  and place yourself in the folder containing the 3 Dockerfiles (client_Dockerfile, server1_Dockerfile, server2_Dockerfile).  
Then run the following commands
## Build Client
```
sudo docker build -t pp-traclus-client -f client_Dockerfile .
```
## Build Server 1
```
sudo docker build -t pp-traclus-server1 -f server1_Dockerfile .
```
## Build Server 2
```
sudo docker build -t pp-traclus-server2 -f server2_Dockerfile .
```
# Run Locally
After building the containers successfully, you have the option to run them locally.
## Run Client
```
sudo docker run -p 5002:5555 -it --rm pp-traclus-client
```
## Run Server 1
```
sudo docker run -p 5005:5555 -it --rm pp-traclus-server1
```
## Run Server 2
```
sudo docker run -p 5010:5555 -it --rm pp-traclus-server2
```
# Push to IBM
Or you can push them to the IBM clouds.

## Client
```
sudo docker tag pp-traclus-client de.icr.io/papaya-de/pp-traclus-client:latest
sudo docker push de.icr.io/papaya-de/pp-traclus-client:latest
```
## Server 1
```
sudo docker tag pp-traclus-server1 de.icr.io/papaya-de/pp-traclus-server1:latest
sudo docker push de.icr.io/papaya-de/pp-traclus-server1:latest
```
## Server 2
```
sudo docker tag pp-traclus-server2 de.icr.io/papaya-de/pp-traclus-server2:latest
sudo docker push de.icr.io/papaya-de/pp-traclus-server2:latest
```

After activating the server1 and server2 on the papaya platform, we need to run the client locally.
```
sudo docker run -p 5002:5555 -it --rm pp-traclus-client-v3
```
# Init parameters
Once we run the containers (either in client-server mode or two non-colluding servers mode), we can run clustering algorithm via using those containers.  

Prior to the executing the client 

Input:  
- DS: Dataset containing line segments. The dataset format should be (X1,X2,Y1,Y2) for each line, otherwise it won't be accepted.   
- EPS: Epsilon   
- MS: MinLns   
- E: Number Of line segments to cluster   
- IT: Number of iterations   

## Two Non-colluding Servers mode (Client/Server1/Server2)
```
curl http://0.0.0.0:5002/init0/SERVER1_IP/SERVER1_PORT/SERVER1_URL/SERVER2_IP/SERVER2_PORT/SERVER2_URL
```

## To use Client/Server mode
```
curl http://0.0.0.0:5002/init1/SERVER1_IP/SERVER1_PORT/SERVER1_URL
```


# Run Clustering
Then we can start the clustering, a python function will detect the used mode and either the servers are running locally or on the IBM clouds and adapt to it.
```
curl -F 'file=@DS' http://0.0.0.0:5002/start/E/MS/EPS/IT
```

# Benchmark

## Client/Server1 Running on Argentera

Dataset: lines_partitioned_test.csv
Epsilon : 4500000000
Minlns : 3

| Line Segments   | Time(1 iter)| RAM(1 iter) | Time(3 iter)| RAM(3 iter) |
|----------------:|:-----------:|:-----------:|:-----------:|:-----------:|
| 100             | 0:01:06     |  1.85GB     | 0:01:15     |  2GB        |
| 400             | 0:13:34     |  10.3GB     | 0:16:40     | 11GB        | 
| 1000            | 1:27:24     |    58GB     | 1:53:12     | 62GB        |



## Client/Server1/Server2 Running on Argentera/IBM/IBM

<p align="center">
<img src="data/tests/ibm/time.png"/>
</p>


| Line Segments   | Time(1 Iter)| 
|----------------:|:-----------:|
| 100             | 0:02:53     | 
| 400             | 0:34:17     | 
| 1000            | 3:30:35     |


