#include "tarball.h"
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <thread>

// Write an archive with multiple entries of the same file to stdout
// unpack the archive by piping it to unix command tar
// ./example02 | tar xv --record-size=512
//

int main(int argc, char **argv) {
  (void)argc;
  (void)argv;
  /* create the tar file */
  lindenb::io::Tar tarball(std::cout);
  /* add item 1 */
  tarball.put("myfiles/item1.txt", "Hello World 1\n");
  /* add item 2 */
  int i = 0;
  while (true) {
    std::stringstream hello;
    hello << "Hello " << std::setfill('0') << std::setw(5) << i++ << " world\n";
    if (!tarball.put("item.txt", hello.str().c_str())) {
      std::cerr << "Put failed" << std::endl;
    }
    std::cout.flush();
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  /* finalize the tar file */
  tarball.finish();

  /* close the file */
  // stdout.close();
  /* we're done */
  return EXIT_SUCCESS;
  ;
}
