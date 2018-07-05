#include <osapi/ScoreSystem/ScoreSystemCrtl.hpp>
#include <osapi/Thread.hpp>
#include <sys/types.h>
#include <unistd.h>

int main()
{
	// FORKING PROCESS TO AVOID LOCKING SYSTEMCTL AND CAUSING A CRASH
	pid_t pid;
	pid = fork();

	if (pid != 0) // IF NOT CHILD, REBOOT ON ERROR, RETURN ON OK
	{
		if (pid == -1) system("reboot");
		else return 0;
	}

	ScoreSystemCrtl scr;

	osapi::Thread scrt(&scr);

	scrt.start();

	scrt.join();
}
