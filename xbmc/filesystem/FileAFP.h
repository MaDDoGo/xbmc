/*
 *      Copyright (C) 2011 Team XBMC
 *      http://www.xbmc.org
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *  http://www.gnu.org/copyleft/gpl.html
 *
 */

// FileAFP.h: interface for the CFileAFP class.
#ifndef FILEAFP_H_
#define FILEAFP_H_


#ifdef _LINUX

#include "IFile.h"
#include "URL.h"
#include "threads/CriticalSection.h"

// libafpclient includes
#ifdef __cplusplus
extern "C" {
#endif
#ifdef __cplusplus
}
#endif

CStdString URLEncode(CStdString str);

struct libafpclient;
struct afp_server;
struct afp_file_info;
struct afp_volume;
struct afp_url;
class  DllLibAfp;

class CAfpConnection : public CCriticalSection
{
public:
    enum eAfpConnectError
    {
      AfpOk     = 0,
      AfpFailed = 1,
      AfpAuth   = 2,
    };
    typedef enum eAfpConnectError afpConnnectError;

   CAfpConnection();
  ~CAfpConnection();

  afpConnnectError      Connect(const CURL &url);
  void                  Disconnect(void);
  struct afp_server     *GetServer()    {return m_pAfpServer;}
  struct afp_volume     *GetVolume()    {return m_pAfpVol;};
  struct afp_url        *GetUrl()       {return m_pAfpUrl;};
  CStdString            GetPath(const CURL &url);
  DllLibAfp             *GetImpl()      {return m_pLibAfp;}

private:
  bool                  initLib(void);
  bool                  connectVolume(const char *volumename);
  void                  disconnectVolume(void);
  CStdString            getAuthenticatedPath(const CURL &url);

  int                   m_OpenConnections;
  int                   m_LastActive;
  struct afp_server     *m_pAfpServer;
  struct afp_volume     *m_pAfpVol;
  struct afp_url        *m_pAfpUrl;
  struct libafpclient   *m_pAfpClient;
  DllLibAfp             *m_pLibAfp;
  bool                  m_bDllInited;
};

extern CAfpConnection gAfpConnection;

namespace XFILE
{
class CFileAFP : public IFile
{
public:
  CFileAFP();
  virtual ~CFileAFP();
  virtual void          Close();
  virtual int64_t       Seek(int64_t iFilePosition, int iWhence = SEEK_SET);
  virtual unsigned int  Read(void* lpBuf, int64_t uiBufSize);
  virtual bool          Open(const CURL& url);
  virtual bool          Exists(const CURL& url);
  virtual int           Stat(const CURL& url, struct __stat64* buffer);
  virtual int           Stat(struct __stat64* buffer);
  virtual int64_t       GetLength();
  virtual int64_t       GetPosition();
  virtual int           Write(const void* lpBuf, int64_t uiBufSize);

  virtual bool          OpenForWrite(const CURL& url, bool bOverWrite = false);
  virtual bool          Delete(const CURL& url);
  virtual bool          Rename(const CURL& url, const CURL& urlnew);
  virtual int           GetChunkSize() {return 1;}
  // implement iocontrol for seek_possible for preventing the stat in File class for
  // getting this info ...
  virtual int           IoControl(EIoControl request, void* param)
                        { if (request == IOCTRL_SEEK_POSSIBLE) return 1;
                          return -1;
                        };

protected:
  bool                  IsValidFile(const CStdString& strFileName);

  CURL                  m_url;
  int64_t               m_fileSize;
  off_t                 m_fileOffset; // current SEEK pointer
  struct afp_file_info *m_pFp;
};
}
#endif // _LINUX
#endif // FILEAFP_H_
