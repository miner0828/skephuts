# skephuts

Skephuts is a command line utility for finding witch/swamp huts in Minecraft 1.16.


### Installation

```sh
make
(optional for system-wide) sudo make install
```


### Usage

skephuts [options] \<seed\>
   
Valid options:
   
      -h      Prints this message.
   
      -d      Includes the average distance from one witch hut to another
   
      -o      Includes the distance from the first witch hut to the origin.
   
      -x      Set minimum X value that will be searched (in regions of 512x512 blocks). Default: -100
   
      -X      Set maximum X value that will be searched (in regions of 512x512 blocks). Default: 100
   
      -z      Set minimum Z value that will be searched (in regions of 512x512 blocks). Default: -100
   
      -Z      Set maximum Z value that will be searched (in regions of 512x512 blocks). Default: 100
   
      -D      Show double huts
   
      -T      Show triple huts

	  -n      Specifies a non-numeric seed. Seed is acquired during runtime.
  
 
 ### Examples
 
 ```sh
skephuts -x-250 -X250 -z20 -Z100 -o -d 8054221181584912980 -D -T

skephuts 8163259126493571 (quad)

skephuts -D -T -n
Seed: random non-numeric seed
 ```
