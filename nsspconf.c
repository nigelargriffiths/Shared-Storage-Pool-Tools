#include <stdio.h>
#include <stdlib.h>
#include <libperfstat.h>

perfstat_ssp_t *ssp_global;
perfstat_ssp_t *ssp_disk;
perfstat_ssp_t *ssp_lu;
perfstat_ssp_t *ssp_node;

int global_count;
int disk_count;
int lu_count;
int node_count;

int vios = 0;

void error(char* where)
{
	perror(where);
	exit(-7);
}

int main(int argc, char* argv[])
{
int i;
int rc;
int c;
        while((c = getopt(argc, argv, "hv"))!= EOF){
                switch(c){
		default: 
                case 'h':
			printf("Hint: %s [-v] [-h]\n");
			printf("\tList Shared Storage Pool info: global, disk including tier & failure group, LU including maps to client LPARs\n",argv[0]);
			printf("\tSeem to ignore LU's that are not mapped to a client LPAR\n");
			printf("\t -v \tinclude VIOS details. Warning: This can add 2 seconds per VIOS\n");
			printf("\t -h \tThis help info and stop\n");
                        exit(0);
                case 'v':
			vios = 1;
                        break;
		}
	}

	/* Phase 1: Enable the cluster statistics */
	if( perfstat_config(PERFSTAT_ENABLE|PERFSTAT_CLUSTER_STATS, NULL) < 0) 
		error("perfstat_config SSP is not available. Only run this on a VIOS 2.2.6+ with a Shared Storeage Pool");

	/* Phase 2: Enable the cluster statistics using perfstat_config */
	if( (global_count = perfstat_ssp(NULL, NULL, sizeof(perfstat_ssp_t),0,SSPGLOBAL) ) < 0)
		perror("perfstat_ssp(global init)");
	if( (disk_count   = perfstat_ssp(NULL, NULL, sizeof(perfstat_ssp_t),0,SSPDISK) ) < 0)
		error("perfstat_ssp(disk init)");
	if( (lu_count     = perfstat_ssp(NULL, NULL, sizeof(perfstat_ssp_t),0,SSPVTD) ) < 0) 
		error("perfstat_ssp(lu init)");

	/* printf("Global=%d Disk=%d lu=%d\n",global_count,disk_count,lu_count); */

	/* Phase 3: Prepare memory buffers */
	ssp_global = (perfstat_ssp_t *) malloc(sizeof(perfstat_ssp_t) * global_count);
	ssp_disk   = (perfstat_ssp_t *) malloc(sizeof(perfstat_ssp_t) * disk_count);
	ssp_lu     = (perfstat_ssp_t *) malloc(sizeof(perfstat_ssp_t) * lu_count);
	if(ssp_global == (perfstat_ssp_t *)NULL || ssp_disk == (perfstat_ssp_t *)NULL || ssp_lu == (perfstat_ssp_t *)NULL ) 
		error("malloc failure requesting space to store perfstat data");

	/* Phase 4: Collect the data and display it */
	if( (rc = perfstat_ssp(NULL, ssp_global, sizeof(perfstat_ssp_t),global_count,SSPGLOBAL) ) <0) 
		error("perfstat_ssp(SSPGLOBAL)");
	
	printf("Global ");
	printf("ClusterName=%s ", ssp_global->cluster_name);
	printf("PoolName=%s ", ssp_global->spool_name);
	printf("TotalSpace=%lld ", ssp_global->u.global.total_space);
	printf("TotalUsedSpace=%lld\n", ssp_global->u.global.total_used_space);

	if( (rc = perfstat_ssp(NULL, ssp_disk, sizeof(perfstat_ssp_t),disk_count,SSPDISK) ) < 0) 
		error("perfstat_ssp(SSPDISK)");
	
	for(i=0; i<rc; i++){
		printf("DiskName=%s ", ssp_disk[i].u.disk.diskname);
		printf("capacity=%lld free=%lld tier=%s fg=%s\n",
			ssp_disk[i].u.disk.capacity,
			ssp_disk[i].u.disk.free,
			ssp_disk[i].u.disk.tiername,
			ssp_disk[i].u.disk.fgname);
	}

	if( (rc = perfstat_ssp(NULL, ssp_lu, sizeof(perfstat_ssp_t),lu_count,SSPVTD) ) < 0) 
		error("perfstat_ssp(SSPLU)");

	for(i=0; i<rc; i++){
		printf("LU=%s type=%s size=%lld free=%lld usage=%lld client_id=%d mtm=%s VTD=%s DRC=%s udid=%s\n",
			ssp_lu[i].u.vtd.lu_name,
			ssp_lu[i].u.vtd.lu_type,
			ssp_lu[i].u.vtd.lu_size,
			ssp_lu[i].u.vtd.lu_free,
			ssp_lu[i].u.vtd.lu_usage,
			ssp_lu[i].u.vtd.client_id,
			ssp_lu[i].u.vtd.mtm,
			ssp_lu[i].u.vtd.vtd_name,
			ssp_lu[i].u.vtd.drcname,
			ssp_lu[i].u.vtd.lu_udid);
	}

	/* Phase 5: output VIOS details - warning this can take 2 seconds per VIOS and
		if VIOS(s) are shutdown or they have network or pool issues then libperfstat will output annoying vague errors */
	if(vios) {
		if( (node_count     = perfstat_ssp(NULL, NULL, sizeof(perfstat_ssp_t),0,SSPNODE) ) < 0) 
			error("perfstat_ssp(node init)");
		
		if( (ssp_node   = (perfstat_ssp_t *) malloc(sizeof(perfstat_ssp_t) * node_count)) == (perfstat_ssp_t *)NULL )
			error("malloc failure");
		
		if( (rc = perfstat_ssp(NULL, ssp_node, sizeof(perfstat_ssp_t),node_count,SSPNODE) ) < 0) 
			error("perfstat_ssp(SSPNODE)");
		
		for(i=0; i<rc; i++){
			printf("VIOS=%s ", ssp_node[i].u.node.hostname);
			printf("IP=%s ", ssp_node[i].u.node.ip);
			printf("MTMS=%s ", ssp_node[i].u.node.mtms);
			printf("lparid=%d ", ssp_node[i].u.node.lparid);
			printf("ioslevel=%s ", ssp_node[i].u.node.ioslevel);
			printf("status=%s ",      (ssp_node[i].u.node.status==1?"OK":"-"));
			printf("poolstatus=%s\n", (ssp_node[i].u.node.poolstatus==1?"OK":"-"));
		}
	}

	/* Phase 6: disable cluster statistics */
	perfstat_config(PERFSTAT_DISABLE|PERFSTAT_CLUSTER_STATS, NULL);
	return 0;
}
