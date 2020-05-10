# Shared Storage Pools Tools
- These scripts and code are offers "as-is" no warrenteee what-so-ever.
- As they are so small they a not cotyrighted.
- The scripts are simple but I find them very useful.
- The nslim program can destroy your SSP data so be careful.
- I hope you find them useful too.

Chears, Twitter user @mr_nmon, Nigel Griffiths.


Get usage information from my AIXpert Blog
- httlps://www.ibm.com/support/pages/shared-storage-pools-hands-fun-virtual-disks-lu-example


Ksh shell scipts can be run as padmin user on the VIOS
-  ncluster	status of all VIOS
-  nlu		improved lu replacement
-  npool  	storage pool use
-  nmap   	finds if a lu is online (mapped) on any VIOS

Binary program - here compiled on AIX 6 which is goood for any current VIOS level
-  nslim	copy a THICK provisoned LU or backup file to a new THIN provisioned LU plus nslim source code nslim_v4.c  Compile it we full optimisation for speed.

WARNING: For details on how to use nslim you MUST read - mistakes could corrupt your SSP
- httlps://www.ibm.com/support/pages/shared-storage-pools-hands-fun-virtual-disks-lu-example


This version includes the new -V option.
## nsllin usage
```
$ ./nslim -?
Usage: ./nslim (v4) is a filter style program using stdin & stdout
        It will thinly write a file (only copy non-zero blocks)
        It uses 1MB blocks
        If a block is zero-filled then it is skipped using lseek()
        If a block has data then it will write() the block unchanged
Example:
        ./nslim   <AIX.lu   >SSP-LU-name
Flags:
        -v for verbose output for every block you get a W=write or .=lseek on stderr
        -V for verbose output on each GB you get count of written or skipped blocks
                ./nslim -v   <AIX.lu   >SSP-LU-name
                 this gives you visual feedback on progress
        -t like verbose but does NOT actually write anything to stdout
                 this lets you passively see the mix of used and unused blocks
                ./nslim -t   <AIX.lu
        -h or -? outputs this helpful message!
Warning:
         Get the redirection wrong and you will destroy your LU data
```

## ncluster usage
- ncluster - can take 10 seconds to run

Example output
```
$ ncluster
No State Repos Pool Role ---Upgrade-Status--- Node-Name
 1    OK    OK   OK  DBN    2.2.4.10 ON_LEVEL bronzevios1.aixncc.uk.ibm.com
 2    OK    OK   OK         2.2.4.10 ON_LEVEL silvervios1.aixncc.uk.ibm.com
 3    OK    OK   OK         2.2.4.10 ON_LEVEL goldvios1.aixncc.uk.ibm.com
 4    OK    OK   OK         2.2.4.10 ON_LEVEL orangevios1.aixncc.uk.ibm.com
 5    OK    OK   OK         2.2.4.10 ON_LEVEL redvios1.aixncc.uk.ibm.com
```

- ON_LEVEL = good
- UP_LEVEL means not yet all VOIS's on higher ioslevel
- DBN= database node = the one that writes to the repository


## nln
- nlu - improved lu replacement
- Better layout
- You can order by each colonm

- 10 lines of ksh, based on lu -list -field
```
$ nlu -?
nlu [-sizemb | -usedmb | -used | -type | -tier | -name (default)]

$ nlu
 SizeMB  UsedMB Used%  Type  Tier Name
  32768    3392   10%  THIN SYSTEM AIX735_b
      1       1  100% THICK SYSTEM JUNK2
 131072  130307   99%  THIN SYSTEM blue_backupvg
 131072   53253   40%  THIN SYSTEM blue_rootvg
 131072   51460   39%  THIN SYSTEM blue_scratchvg
 131072   99588   75%  THIN SYSTEM blue_webvg
  65536       0    0%  THIN SYSTEM emerald3
  65536    6369    9%  THIN SYSTEM emerald3
  65536   65536  100% THICK SYSTEM orange5a
  65536   13317   20%  THIN SYSTEM purple3boot
1048576  971779   92%  THIN SYSTEM purple3files
  32768   14356   43%  THIN SYSTEM ruby32boot
  65536   40773   62%  THIN SYSTEM ruby32data1
  65536    1144    1%  THIN SYSTEM vm100

$ nlu -Usedmb
 SizeMB  UsedMB Used%  Type  Tier Name
. . . 
  65536    1144    1%  THIN SYSTEM vm100
  32768    2561    7%  THIN SYSTEM vm91_mirror
  32768    3057    9%  THIN SYSTEM vm61c
  32768    3063    9%  THIN SYSTEM vm36
  32768    3063    9%  THIN SYSTEM vm61a
  32768    3392   10%  THIN SYSTEM AIX735_b
  32768    3392   10%  THIN SYSTEM volume-vm112
  32768    3812   11%  THIN SYSTEM volume-vm23boot
  32768    5383   16%  THIN SYSTEM volume-vm91
  65536    6369    9%  THIN SYSTEM emerald3
  65536    7303   11%  THIN SYSTEM vm18boot
  65536    8872   13%  THIN SYSTEM vm23a
  65536    9944   15%  THIN SYSTEM vm24a
  65536   13317   20%  THIN SYSTEM purple3boot
  32768   14356   43%  THIN SYSTEM ruby32boot
 131072   16331   12%  THIN SYSTEM vm27b
  65536   19050   29%  THIN SYSTEM vm21ahh  
```


## npool - storage pool use
```
$ npool
name = globular
Pool Pacific
Pool-Size= 4192256 MB
Pool-Used= 2722584 MB =64.94%
Pool-Free= 1469672 MB =35.06%

Allocated to client VMs   = 4924417 MB
Allocated compared to Pool=117.46%
Used to Allocate Ratio =55.29%
Overcommit=3495928 MB
```


## nmap - Is a LU mapped to a vhost somewhere
```
$ nmap -h
Nigel's nmap to find if a LU (virtual disk) is mapped anywhere on the SSP to a LPAR/VM
nmap  -h
nmap  lu-name
nmap  ALL - lists all LUs on every VIOS


$ nmap vm22
Seach the SSP for vm22
NODE diamondvios2.aixncc.uk.ibm.com
NODE diamondvios1.aixncc.uk.ibm.com
NODE greenvios2.aixncc.uk.ibm.com
NODE greenvios1.aixncc.uk.ibm.com
NODE limevios2.aixncc.uk.ibm.com
0x0000000b,vhost6:U8284.22A.215296V-V4-C8:vm22a.5cc155a78dbc3d1b5865128a70ec0788
NODE limevios1.aixncc.uk.ibm.com
0x0000000b,vhost7:U8284.22A.215296V-V1-C6:vm22a.5cc155a78dbc3d1b5865128a70ec0788
NODE purplevio2.aixncc.uk.ibm.com
NODE purplevio1.aixncc.uk.ibm.com
NODE rubyvios2.aixncc.uk.ibm.com
NODE emeraldvios2.aixncc.uk.ibm.com
NODE emeraldvios1.aixncc.uk.ibm.com
NODE rubyvios1.aixncc.uk.ibm.com
NODE indigovios1.aixncc.uk.ibm.com
```

- Note 0x0000000b column is the Client LPAR/VM ID as found on the HMC

# Nigel SSP config - nsspconf
```
./nsspconf_VIOS3
Global ClusterName=orbit PoolName=orbit TotalSpace=6289408 TotalUsedSpace=2784136

DiskName=hdisk4 capacity=393216 free=393152 tier=SYSTEM fg=v7000_tan
DiskName=hdisk5 capacity=393216 free=393152 tier=SYSTEM fg=v7000_tan
DiskName=hdisk6 capacity=393216 free=393152 tier=SYSTEM fg=v7000_tan
. . . 
LU=volume-vm220-a74b3c63-0000002e-boot-0-ef8bc3b7-2ff1 type=THIN_LU size=262144 free=168212 usage=93948 client_id=23 mtm=9009-22A067804940 VTD=vtscsi0 DRC=U9009.22A.7804940-V3-C47 udid=062d372aacffa30d81c5a73aa349bc2a
LU=silver5 type=THIN_LU size=65536 free=64699 usage=840 client_id=4 mtm=9009-42A067804930 VTD=vtscsi0 DRC=U9009.42A.7804930-V1-C4 udid=fa670d2db6392eeef2fed0433a024571
LU=vm16boot type=THIN_LU size=65536 free=34859 usage=26183 client_id=5 mtm=8408-E8E0221D494V VTD=vtscsi0 DRC=U8408.E8E.21D494V-V2-C16 udid=ec95b680d1e0ce7354fe56fe05ee09e0
LU=silver5 type=THIN_LU size=65536 free=64699 usage=840 client_id=4 mtm=9009-42A067804930 VTD=vtscsi0 DRC=U9009.42A.7804930-V2-C4 udid=fa670d2db6392eeef2fed0433a024571
```
