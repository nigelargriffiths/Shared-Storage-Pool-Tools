#define _LARGEFILE64_SOURCE     /* See feature_test_macros(7) */
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include <sys/types.h>
#include <unistd.h>

#define SSP_BLOCK_SIZE (1048576)  /* 1 MB */

int main(int argc, char **argv)
{
	int	ret;
	int	verbose = 0;
	int	counting = 0;
	int	test = 0;
	char	*zero;
	char	*temp;
	long	count = SSP_BLOCK_SIZE; /* using 1MB blocks so make the buffer this size */
	int	written = 0;
	int	skipped = 0;
	int	totalw = 0;
	int	totals = 0;

	while ( -1 != (ret = getopt(argc, argv, "?htvV" ))) {
		switch (ret) {
		case '?':
		case 'h':
			fprintf(stderr, "Usage: %s (v4) is a filter style program using stdin & stdout\n", argv[0]);
			fprintf(stderr, "\tIt will thinly write a file (only copy non-zero blocks)\n");
			fprintf(stderr, "\tIt uses %dMB blocks\n",SSP_BLOCK_SIZE/1048576);
			fprintf(stderr, "\tIf a block is zero-filled then it is skipped using lseek()\n");
			fprintf(stderr, "\tIf a block has data then it will write() the block unchanged\n");
			fprintf(stderr, "Example:\n");
			fprintf(stderr, "\t%s   <AIX.lu   >SSP-LU-name\n", argv[0]);
			fprintf(stderr, "Flags:\n");
			fprintf(stderr, "\t-v for verbose output for every block you get a W=write or .=lseek on stderr\n");
			fprintf(stderr, "\t-V for verbose output on each GB you get count of written or skipped blocks\n");
			fprintf(stderr, "\t\t%s -v   <AIX.lu   >SSP-LU-name\n", argv[0]);
			fprintf(stderr, "\t\t this gives you visual feedback on progress\n");
			fprintf(stderr, "\t-t like verbose but does NOT actually write anything to stdout\n");
			fprintf(stderr, "\t\t this lets you passively see the mix of used and unused blocks\n");
			fprintf(stderr, "\t\t%s -t   <AIX.lu\n", argv[0]);
			fprintf(stderr, "\t-h or -? outputs this helpful message!\n");
/*			fprintf(stderr, "Memory use:\n");
			fprintf(stderr, "\t%s needs 128+ MB of memory. To stop ulimit issues use: ulimit -d 130000\n", argv[0]);
*/
			fprintf(stderr, "Warning:\n");
			fprintf(stderr, "\t Get the redirection wrong and you will destroy your LU data\n");
			exit(42);
		case 't':
			test++; /* fall through */
		case 'v':
			verbose++;
			break;
		case 'V':
			counting++;
			break;
		}
	}

	temp = malloc(count); /* create the I/O buffer */
	zero = malloc(count); /* create Zero fill block */
	if(temp == NULL || zero == NULL ) {
		perror("null returned from malloc()");
		exit(99);
	}
	memset( zero, 0, count);
	if(verbose && counting)
		verbose = 0;
	if (verbose)
		fprintf(stderr, "Processing\n");
	if (counting)
		fprintf(stderr, "Counting\n");
	while(1) {
		ret = read(0, temp, count) ;
		if ( ret == 0) {
			break;
		}
		if ( ret == -1) {
			perror("read error");
			exit(43);
		}
		/* search for nonzero characters using memory compare function */
		if (memcmp(zero, temp, count) == 0) {
			skipped++;
			if (verbose) 
				fprintf(stderr, ".");
			if ( !test ) {
				if ( (ret = lseek64(1, (off_t)count, SEEK_CUR) ) == -1) {
					perror("lseek error");
					exit(45);
				}
			}
		} else {
			written++;
			if (verbose) 
				fprintf(stderr, "W");
			if ( !test ) {
				if ( (ret = write(1, temp, count) ) == -1) {
					perror("write error");
					exit(44);
				}
			}
		}
		if(counting) {
			if(skipped + written == 1024) {
				fprintf(stderr, "%3d Written=%4d Skipped=%4d\n",counting,written,skipped);
				counting++;
				totalw += written;
				totals += skipped;
				written = 0;
				skipped = 0;
			}
		}
	}
	if(counting) {
		if(skipped + written > 0) {
			fprintf(stderr, "%3d Written=%4d Skipped=%4d\n",counting,written,skipped);
		}
		fprintf(stderr, "Percent Written=%.2f%% Skipped=%.2f%%\n",
			(float)totalw * 100.0 / (float)(totalw + totals),
			(float)totals * 100.0 / (float)(totalw + totals) );
	}
	if (verbose || counting) 
		fprintf(stderr, "\nDone\n");
	return 0;
}
