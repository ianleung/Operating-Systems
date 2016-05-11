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
//source www.gnu.org/software/libc/manual/html_node/Permission-Bits.html

int
main(int argc, char *argv[])
{
    extern char *optarg;
    int option = getopt(argc, argv, "u:c:l:");
    DIR *p_dir;
	struct dirent *p_dirent;
	struct stat buf;
  
    if (argc != 3){
        printf("Usage of myls: %s [-ucl] directory\n", argv[0]);
        exit(0);
	}
    //open file
    if ((p_dir = opendir(argv[2])) == NULL) {
        printf("opendir(%s) failed\n", argv[2]);
        exit(2);
    }
    while ((p_dirent = readdir(p_dir)) != NULL) {
		//set path
        char *strpath = p_dirent->d_name;      // relative path name!
		char *fullpath = malloc(2 + strlen(optarg) + strlen(strpath)); //size of path with link
		char *path = strpath;
		sprintf(fullpath, "%s/%s", optarg, strpath);
		if (strpath == NULL) {
            printf("Null pointer found!");
            exit(1);
		}
        char access[] = "----------\0";
		if (lstat(fullpath, &buf) < 0){
			perror("lstat error");
			continue;
		}
        mode_t mode = buf.st_mode;
		if(S_ISREG(buf.st_mode))  access[0]= '-'; //set first letter
        else if(S_ISDIR(buf.st_mode))  access[0] = 'd';
		#ifdef S_ISLNK
        else if(S_ISLNK(buf.st_mode)){
			//if symbolic link set path 
			access[0] = 'l';
			char targetpath[256];
			readlink(fullpath, targetpath, sizeof(targetpath));
			path = malloc(2 + strlen(strpath) + strlen(targetpath) );
			sprintf(path,"%s->%s", strpath, targetpath);
		}
		#endif
        //print access
        access[1] = (mode & S_IRUSR) ? 'r' : '-'; //Read permission bit for the owner of the file
        access[2] = (mode & S_IWUSR) ? 'w' : '-'; //write
        access[3] = (mode & S_IXUSR) ? 'x' : '-'; //execute
        access[4] = (mode & S_IRGRP) ? 'r' : '-'; //Read permission bit for the group owner of the file.
        access[5] = (mode & S_IWGRP) ? 'w' : '-'; //w
        access[6] = (mode & S_IXGRP) ? 'x' : '-'; //e
        access[7] = (mode & S_IROTH) ? 'r' : '-'; //Read permission bit for other users.
        access[8] = (mode & S_IWOTH) ? 'w' : '-'; //w
        access[9] = (mode & S_IXOTH) ? 'x' : '-'; //e
		printf(" %s ",access);
		//print user and group
        struct passwd *password = getpwuid(buf.st_uid);
        struct group *group = getgrgid(buf.st_gid);
		char *userName = password ->pw_name;
		char *groupName = group->gr_name;
		printf(" %s ",userName);
		printf(" %s ",groupName);
		//print time
		time_t t = time(NULL);
		struct tm tm = *localtime(&t);
        struct tm *lastaccesstime;
        char timeaccessed[256];
		printf(" %6jd ",buf.st_size);
		switch(option){
        	case 'u':
        		lastaccesstime = localtime(&buf.st_atime);
        		break;
        	case 'c':
                lastaccesstime = localtime(&buf.st_mtime);
                break;
            case 'l':
                lastaccesstime = localtime(&buf.st_ctime);
                break;
        }
		if(tm.tm_year == lastaccesstime->tm_year){
			strftime(timeaccessed, sizeof(timeaccessed),"%b %e %H:%M ",lastaccesstime);
		}
		else{
			strftime(timeaccessed, sizeof(timeaccessed),"%b %e %Y ",lastaccesstime);
		}
		printf(" %s ",timeaccessed);
		//print path
		printf(" %s ",path);
        printf("\n");
    }
    return 0;
}

