
/***************************************************************************
 * nmap_error.cc -- Some simple error handling routines.                   *
 *                                                                         *
 ***********************IMPORTANT NMAP LICENSE TERMS************************
 *                                                                         *
 * The Nmap Security Scanner is (C) 1996-2019 Insecure.Com LLC ("The Nmap  *
 * Project"). Nmap is also a registered trademark of the Nmap Project.     *
 * This program is free software; you may redistribute and/or modify it    *
 * under the terms of the GNU General Public License as published by the   *
 * Free Software Foundation; Version 2 ("GPL"), BUT ONLY WITH ALL OF THE   *
 * CLARIFICATIONS AND EXCEPTIONS DESCRIBED HEREIN.  This guarantees your   *
 * right to use, modify, and redistribute this software under certain      *
 * conditions.  If you wish to embed Nmap technology into proprietary      *
 * software, we sell alternative licenses (contact sales@nmap.com).        *
 * Dozens of software vendors already license Nmap technology such as      *
 * host discovery, port scanning, OS detection, version detection, and     *
 * the Nmap Scripting Engine.                                              *
 *                                                                         *
 * Note that the GPL places important restrictions on "derivative works",  *
 * yet it does not provide a detailed definition of that term.  To avoid   *
 * misunderstandings, we interpret that term as broadly as copyright law   *
 * allows.  For example, we consider an application to constitute a        *
 * derivative work for the purpose of this license if it does any of the   *
 * following with any software or content covered by this license          *
 * ("Covered Software"):                                                   *
 *                                                                         *
 * o Integrates source code from Covered Software.                         *
 *                                                                         *
 * o Reads or includes copyrighted data files, such as Nmap's nmap-os-db   *
 * or nmap-service-probes.                                                 *
 *                                                                         *
 * o Is designed specifically to execute Covered Software and parse the    *
 * results (as opposed to typical shell or execution-menu apps, which will *
 * execute anything you tell them to).                                     *
 *                                                                         *
 * o Includes Covered Software in a proprietary executable installer.  The *
 * installers produced by InstallShield are an example of this.  Including *
 * Nmap with other software in compressed or archival form does not        *
 * trigger this provision, provided appropriate open source decompression  *
 * or de-archiving software is widely available for no charge.  For the    *
 * purposes of this license, an installer is considered to include Covered *
 * Software even if it actually retrieves a copy of Covered Software from  *
 * another source during runtime (such as by downloading it from the       *
 * Internet).                                                              *
 *                                                                         *
 * o Links (statically or dynamically) to a library which does any of the  *
 * above.                                                                  *
 *                                                                         *
 * o Executes a helper program, module, or script to do any of the above.  *
 *                                                                         *
 * This list is not exclusive, but is meant to clarify our interpretation  *
 * of derived works with some common examples.  Other people may interpret *
 * the plain GPL differently, so we consider this a special exception to   *
 * the GPL that we apply to Covered Software.  Works which meet any of     *
 * these conditions must conform to all of the terms of this license,      *
 * particularly including the GPL Section 3 requirements of providing      *
 * source code and allowing free redistribution of the work as a whole.    *
 *                                                                         *
 * As another special exception to the GPL terms, the Nmap Project grants  *
 * permission to link the code of this program with any version of the     *
 * OpenSSL library which is distributed under a license identical to that  *
 * listed in the included docs/licenses/OpenSSL.txt file, and distribute   *
 * linked combinations including the two.                                  *
 *                                                                         *
 * The Nmap Project has permission to redistribute Npcap, a packet         *
 * capturing driver and library for the Microsoft Windows platform.        *
 * Npcap is a separate work with it's own license rather than this Nmap    *
 * license.  Since the Npcap license does not permit redistribution        *
 * without special permission, our Nmap Windows binary packages which      *
 * contain Npcap may not be redistributed without special permission.      *
 *                                                                         *
 * Any redistribution of Covered Software, including any derived works,    *
 * must obey and carry forward all of the terms of this license, including *
 * obeying all GPL rules and restrictions.  For example, source code of    *
 * the whole work must be provided and free redistribution must be         *
 * allowed.  All GPL references to "this License", are to be treated as    *
 * including the terms and conditions of this license text as well.        *
 *                                                                         *
 * Because this license imposes special exceptions to the GPL, Covered     *
 * Work may not be combined (even as part of a larger work) with plain GPL *
 * software.  The terms, conditions, and exceptions of this license must   *
 * be included as well.  This license is incompatible with some other open *
 * source licenses as well.  In some cases we can relicense portions of    *
 * Nmap or grant special permissions to use it in other open source        *
 * software.  Please contact fyodor@nmap.org with any such requests.       *
 * Similarly, we don't incorporate incompatible open source software into  *
 * Covered Software without special permission from the copyright holders. *
 *                                                                         *
 * If you have any questions about the licensing restrictions on using     *
 * Nmap in other works, we are happy to help.  As mentioned above, we also *
 * offer an alternative license to integrate Nmap into proprietary         *
 * applications and appliances.  These contracts have been sold to dozens  *
 * of software vendors, and generally include a perpetual license as well  *
 * as providing support and updates.  They also fund the continued         *
 * development of Nmap.  Please email sales@nmap.com for further           *
 * information.                                                            *
 *                                                                         *
 * If you have received a written license agreement or contract for        *
 * Covered Software stating terms other than these, you may choose to use  *
 * and redistribute Covered Software under those terms instead of these.   *
 *                                                                         *
 * Source is provided to this software because we believe users have a     *
 * right to know exactly what a program is going to do before they run it. *
 * This also allows you to audit the software for security holes.          *
 *                                                                         *
 * Source code also allows you to port Nmap to new platforms, fix bugs,    *
 * and add new features.  You are highly encouraged to send your changes   *
 * to the dev@nmap.org mailing list for possible incorporation into the    *
 * main distribution.  By sending these changes to Fyodor or one of the    *
 * Insecure.Org development mailing lists, or checking them into the Nmap  *
 * source code repository, it is understood (unless you specify            *
 * otherwise) that you are offering the Nmap Project the unlimited,        *
 * non-exclusive right to reuse, modify, and relicense the code.  Nmap     *
 * will always be available Open Source, but this is important because     *
 * the inability to relicense code has caused devastating problems for     *
 * other Free Software projects (such as KDE and NASM).  We also           *
 * occasionally relicense the code to third parties as discussed above.    *
 * If you wish to specify special license conditions of your               *
 * contributions, just say so when you send them.                          *
 *                                                                         *
 * This program is distributed in the hope that it will be useful, but     *
 * WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the Nmap      *
 * license file for more details (it's in a COPYING file included with     *
 * Nmap, and also available from https://svn.nmap.org/nmap/COPYING)        *
 *                                                                         *
 ***************************************************************************/

/* $Id$ */

#include "nmap_error.h"
#include "output.h"
#include "NmapOps.h"
#include "xml.h"

#include <errno.h>
#if TIME_WITH_SYS_TIME
# include <sys/time.h>
# include <time.h>
#else
# if HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif

extern NmapOps o;

#ifdef WIN32
#include <windows.h>
#endif /* WIN32 */


#ifndef HAVE_STRERROR
char *strerror(int errnum) 
{
	static char buf[1024];
	sprintf(buf, "your system is too old for strerror of errno %d\n", errnum);
	return buf;
}
#endif

void fatal(const char *fmt, ...) 
{
	time_t timep;
	struct timeval tv;
	va_list  ap;

	gettimeofday(&tv, NULL);
	timep = time(NULL);

	va_start(ap, fmt);
	log_vwrite(LOG_NORMAL | LOG_STDERR, fmt, ap);
	va_end(ap);
	log_write(LOG_NORMAL | LOG_STDERR, "\nQUITTING!\n");

	if (xml_tag_open())
		xml_close_start_tag();
	if (!xml_root_written())
		xml_start_tag("nmaprun");
	/* Close all open XML elements but one. */
	while (xml_depth() > 1) {
		xml_end_tag();
		xml_newline();
	}

	char errbuf[1024];
	va_start(ap, fmt);
	Vsnprintf(errbuf, sizeof(errbuf), fmt, ap);
	va_end(ap);

	if (xml_depth() == 1) {
		xml_start_tag("runstats");
		print_xml_finished_open(timep, &tv);
		xml_attribute("exit", "error");
		xml_attribute("errormsg", "%s", errbuf);
		xml_close_empty_tag();

		print_xml_hosts();
		xml_newline();

		xml_end_tag(); /* runstats */
		xml_newline();

		xml_end_tag(); /* nmaprun */
		xml_newline();
	}

#ifdef _LIB
	PutNmapGlobalFatal(errbuf);
	throw(1);
#else
	exit(1);
#endif // #ifdef _LIB
}

void error(const char *fmt, ...) 
{
	va_list  ap;
	char errbuf[1024];

	va_start(ap, fmt);
	Vsnprintf(errbuf, sizeof(errbuf), fmt, ap);
	va_end(ap);
	log_write(LOG_NORMAL | LOG_STDERR, "%s\n", errbuf);
#ifdef _LIB
	PutNmapGlobalWarning(errbuf);
#endif
}

void pfatal(const char *fmt, ...) 
{
	time_t timep;
	struct timeval tv;
	va_list ap;
	int error_number;
	char errbuf[1024], *strerror_s;

#ifdef WIN32
	error_number = GetLastError();
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, error_number, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&strerror_s, 0, NULL);
#else
	error_number = errno;
	strerror_s = strerror(error_number);
#endif

	gettimeofday(&tv, NULL);
	timep = time(NULL);

	va_start(ap, fmt);
	Vsnprintf(errbuf, sizeof(errbuf), fmt, ap);
	va_end(ap);

	log_write(LOG_NORMAL | LOG_STDERR, "%s: %s (%d)\n",
		errbuf, strerror_s, error_number);

	if (xml_tag_open())
		xml_close_start_tag();
	if (!xml_root_written())
		xml_start_tag("nmaprun");
	/* Close all open XML elements but one. */
	while (xml_depth() > 1) {
		xml_end_tag();
		xml_newline();
	}
	if (xml_depth() == 1) {
		xml_start_tag("runstats");
		print_xml_finished_open(timep, &tv);
		xml_attribute("exit", "error");
		xml_attribute("errormsg", "%s: %s (%d)", errbuf, strerror_s, error_number);
		xml_close_empty_tag();

		print_xml_hosts();
		xml_newline();

		xml_end_tag(); /* runstats */
		xml_newline();

		xml_end_tag(); /* nmaprun */
		xml_newline();
	}

#ifdef WIN32
	HeapFree(GetProcessHeap(), 0, strerror_s);
#endif

	log_flush(LOG_NORMAL);
	fflush(stderr);

#ifdef _LIB
	PutNmapGlobalFatal(errbuf);
	throw(1);
#else
	exit(1);
#endif // #ifdef _LIB
}

/* This function is the Nmap version of perror. It is like pfatal, but it
   doesn't write to XML and it only returns, doesn't exit. */
void gh_perror(const char *fmt, ...) 
{
	va_list ap;
	int  error_number;
	char *strerror_s;
	char errbuf[1024];

#ifdef WIN32
	error_number = GetLastError();
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, error_number, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&strerror_s, 0, NULL);
#else
	error_number = errno;
	strerror_s = strerror(error_number);
#endif

	va_start(ap, fmt);
	Vsnprintf(errbuf, sizeof(errbuf), fmt, ap);
	va_end(ap);
	log_write(LOG_NORMAL | LOG_STDERR, "%s: %s (%d)\n",
		errbuf, strerror_s, error_number);

#ifdef WIN32
	HeapFree(GetProcessHeap(), 0, strerror_s);
#endif

	log_flush(LOG_NORMAL);
	fflush(stderr);

#ifdef _LIB
	PutNmapGlobalWarning(errbuf);
#endif
	return;
}