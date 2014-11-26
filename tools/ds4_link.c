#include <getopt.h>

#include <ds4_bt.h>

int main(int argc, char* argv[]) {
  // getopt variables
  int index;
  int c;
  int delete = 0;

  int rc;

  opterr = 0;

  while ((c = getopt(argc, argv, "d")) != -1) {
    switch (c) {
      case 'd':
        delete = 1;
        break;

      default:
        return -1;
    }
  }


  if (argc != optind + 2 && !delete) {
    printf("./ds4_link <ds4 addr> <key>\n");
    return -1;
  }
  if (argc != optind + 1 && delete) {
    printf("./ds4_link <ds4 addr>\n");
    return -1;
  }

  if (delete) {
    rc = del_bd_key(argv[optind]);
  } else {
    rc = set_bd_key(argv[optind], argv[optind + 1]);
  }
  if (rc != 0) {
    printf("ERROR: Not able to maniuplate bluetooth key\n");
    return -1;
  }

  return 0;
}
