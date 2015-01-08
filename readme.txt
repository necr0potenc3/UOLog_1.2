UOLog Beta 1 Readme

1 What is UOLog?
UOLog is a packet logging tool for Ultima Online.

2 Am I allowed to use UOLog?
You may only use UOLog if you are a shard's administrator or if you have his permission.
You are _not_ allowed to sniff packets on OSI, it's a violation of their Terms of Use (TOS).

3 Why can't I use Attach in Windows 98?
I need the handle of the client's thread. In Windows 98 I can only get it if I am the creator
of the process. In newer Windows versions I can use OpenThread to get the thread handle.
I know there is a way to get the threadhandle in Windows98, if you know it, please tell me.

Quote from MSDN:
"There is no way to get the thread handle from the thread ID. While there is an 
OpenProcess() API that takes a PID and returns the handle to the process, there 
is no corresponding OpenThread() that takes a thread ID and returns a thread 
handle. The reason that the Win32 API does not make thread handles available 
this way is that it can cause damage to an application. The APIs that take a 
thread handle allow suspending/resuming threads, adjusting priority of a thread 
relative to its process, reading/writing registers, limiting a thread to a set 
of processors, terminating a thread, and so forth. Performing any one of these 
operations on a thread without the knowledge of the owning process is dangerous, 
and may cause the process to fail." Source: Microsoft Developer Network, Q127992

However, in Windows 2000 they added OpenThread. No further comments.

4 How do I find the values for clients.cfg?
Check finding_values.html.

5 Sourecode?
Nay, not until it's out of beta.

6 Other
Post in my forums (http://uo.elitecoder.net/phpBB/).

7 Credits
Arog <arog@elitecoder.net> for autoscrolling.
DarkStorm for the idea of using EXCEPTION_SINGLE_STEP instead of the DeBPXs.

--
Folko, 2003, somewhere near Hamburg, Germany.
