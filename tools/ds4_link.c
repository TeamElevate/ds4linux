#include <ds4_bt.h>

int main(const int argc, const char* argv[]) {
  int rc;
  if (argc != 3) {
    printf("./ds4_link <ds4 addr> <key>\n");
    return -1;
  }

  rc = set_bd_key(argv[1], argv[2]);
  if (rc != 0) {
    printf("ERROR: Not able to set set bluetooth key\n");
    return -1;
  }

  return 0;
}
