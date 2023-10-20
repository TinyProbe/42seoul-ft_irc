#ifndef IRCSERV_PROGRAM_H_
#define IRCSERV_PROGRAM_H_

namespace irc {

class Program {
 public:
  static void init(int argc, char **argv);
  static void run();

 private:
  explicit Program();
  explicit Program(Program const &rhs);

  static int argc_;
  static char **argv_;
}; // Program

} // namespace irc

#endif // IRCSERV_PROGRAM_H_
