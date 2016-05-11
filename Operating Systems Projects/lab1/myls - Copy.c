#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>          // for printf().  man 3 printf
#include <stdlib.h>         // for exit().    man 3 exit
#include <pwd.h>            // for getpwuid()
#include <grp.h>            // for getgrgid()
#include <time.h>           // for ctime()
#include <string.h>         // for strlen()

int
main(int argc, char *argv[])
{
	extern char *optarg;

	char *fname;
	int tag;
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);

	DIR *p_dir;
	struct dirent *p_dirent;
	tag = getopt(argc, argv, "u:c:l:");

	//read from opt tags
	if (argc == 3){
		printf("opt found");
		fname = optarg;
	}
	else {
		printf("Usage: %s [-ucl] directory\n", argv[0]);
		exit(1);
	}

	//open file
	if ((p_dir = opendir(fname)) == NULL) {
		printf("opendir(%s) failed\n", fname);
		exit(1);
	}


	while ((p_dirent = readdir(p_dir)) != NULL) {
		char *str_path = p_dirent->d_name;      // relative path name!
		char *full_path = malloc(strlen(fname) + strlen(str_path) + 2);
		char *out_path = str_path;

		sprintf(full_path, "%s/%s", fname, str_path);

		if (str_path == NULL) {
			printf("Null pointer found!");
			exit(1);
		}
		else {

			struct stat buf;
			struct tm *lasttime;
			char timestamp[256];
			char permissions[] = "----------\0";

			if (lstat(full_path, &buf) < 0){
				perror("lstat error");
				continue;
			}
			mode_t mode = buf.st_mode;

			//permissions
			permissions[1] = (mode & S_IRUSR) ? 'r' : '-'; //Read (group owner)
			permissions[2] = (mode & S_IWUSR) ? 'w' : '-'; //Read (group owner)
			permissions[3] = (mode & S_IXUSR) ? 'x' : '-'; //Execute or search (group owner)
			permissions[4] = (mode & S_IRGRP) ? 'r' : '-'; //Read (group owner)
			permissions[5] = (mode & S_IWGRP) ? 'w' : '-'; //Write (group owner)
			permissions[6] = (mode & S_TRWXG) ? 'x' : '-'; //Execute or search (group owner) 
			permissions[7] = (mode & S_IROTH) ? 'r' : '-'; //Read (other users)
			permissions[8] = (mode & S_IWOTH) ? 'w' : '-'; //Write (other users)
			permissions[9] = (mode & S_IXOTH) ? 'x' : '-'; //Execute or search (other users).

			if (S_ISREG(buf.st_mode))  permissions[0] = '-';
			else if (S_ISDIR(buf.st_mode))  permissions[0] = 'd';
#ifdef S_ISLNK
			else if (S_ISLNK(buf.st_mode)){
				permissions[0] = 'l';
				char target_path[256];
				readlink(full_path, target_path, sizeof(target_path));
				out_path = malloc(strlen(str_path) + strlen(target_path) + 2);
				sprintf(out_path, "%s -> %s", str_path, target_path);
			}
#endif


			//timestamp  
			switch (tag){
			case 'u':
				lasttime = localtime(&buf.st_atime);
				break;
			case 'c':
				lasttime = localtime(&buf.st_mtime);
				break;
			case 'l':
				lasttime = localtime(&buf.st_ctime);
				break;
			}


			if (strftime(timestamp, sizeof(timestamp), (tm.tm_year == lasttime->tm_year) ? "%b %_2d %2H:%2M " : "%b %_2d %_5Y ", lasttime) == 0) {
				fprintf(stderr, "strftime returned 0");
				exit(EXIT_FAILURE);
			}

			//names
			struct passwd *pass = getpwuid(buf.st_uid);
			struct group *group = getgrgid(buf.st_gid);

			printf("%s %s %s %6jd %s %s \n", permissions, pass->pw_name, group->gr_name, buf.st_size, timestamp, out_path);
		}

	}

	return 0;
}

