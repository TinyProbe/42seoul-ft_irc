#ifndef IRCSERV_PROGRAM_H_
#define IRCSERV_PROGRAM_H_

namespace irc {

class Program {
 public:
  static void run();

 private:
  explicit Program();
  explicit Program(Program const &rhs);

}; // Program

} // namespace irc

#endif // IRCSERV_PROGRAM_H_
