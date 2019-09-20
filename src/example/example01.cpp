#include "tarball.h"
#include <cstdlib>
#include <fstream>
#include <iostream>

int main(int argc, char **argv) {
  (void)argc;
  (void)argv;
  /* open file for writing */
  std::fstream out("archive.tar", std::ios::out);
  if (!out.is_open()) {
    std::cerr << "Cannot open out" << std::endl;
    return EXIT_FAILURE;
  }
  /* create the tar file */
  lindenb::io::Tar tarball(out);
  /* add item 1 */
  tarball.put("myfiles/item1.txt", "Hello World 1\n");
  /* add item 2 */
  tarball.put("myfiles/item2.txt", "Hello World 2\n");
  /* add a file */
  if (!tarball.putFile("tarfile.cpp", "myfiles/code.cpp")) {
    std::cerr << "Trying to add file failed\n" << std::endl;
  }

  /* finalize the tar file */
  tarball.finish();
  /* close the file */
  out.close();
  /* we're done */
  return EXIT_SUCCESS;
  ;
}
