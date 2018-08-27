/*
	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "Thread.h"
#include "MSConfig.h"

#ifndef _WIN32
#include <signal.h>
#endif

CThread::CThread()
{
#ifdef _WIN32
	m_handle = NULL;
#endif
	m_threadID = 0;
	m_lpThread = NULL;
	m_bRunning = false;
}

CThread::~CThread()
{
	if (m_bRunning)
		StopThread();
	
	
}

#ifdef _WIN32
void CThread::StartThread(LPTHREAD_START_ROUTINE m_cbMain, void* m_args)
#else
void CThread::StartThread(void*(* m_cbMain)(void*), void* m_args)
#endif
{
	// Create the module thread
#ifdef _WIN32
	m_handle = CreateThread(0, 0, m_cbMain, m_args, 0, &m_threadID);

	if (m_handle)
		m_bRunning = true;
#else
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	
	if (!pthread_create(&m_threadID, &attr, m_cbMain, m_args))
		m_bRunning = true;
#endif

	UpdateThreadInformation();	
}

void CThread::StopThread()
{
	UpdateThreadInformation();

	if (!m_bRunning)
		return;

	m_bRunning = false;

	// Terminate the thread
#ifdef _WIN32
	TerminateThread(m_handle, 0);
	m_handle = NULL; //SET THIS TO NULL OTHERWISE THE APP WILL CRASH
#else
	pthread_kill(m_threadID, SIGKILL);
#endif
	m_threadID = 0;	
}

#ifdef _WIN32
DWORD CThread::getThreadID()
#else
pthread_t CThread::getThreadID()
#endif
{
	return m_threadID;
}

bool CThread::IsRunning()
{
	UpdateThreadInformation();

	return m_bRunning;
}

unsigned long CThread::GetExitCode()
{
	return m_exitCode;
}

void CThread::UpdateThreadInformation()
{
#ifdef _WIN32
	if (!m_handle)
		return;

	if (!GetExitCodeThread(m_handle, &m_exitCode))
		return;

	if (m_exitCode == ERROR_STILL_ALIVE)
		m_bRunning = true;
	else
		m_bRunning = false;

#else
	m_bRunning = true;
	m_exitCode = ERROR_LINUX_UNSUPPORTED_RESERVED;
#endif
}
