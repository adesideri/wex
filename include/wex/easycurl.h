/***********************************************************************************************************************
*  WEX, Copyright (c) 2008-2017, Alliance for Sustainable Energy, LLC. All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
*  following conditions are met:
*
*  (1) Redistributions of source code must retain the above copyright notice, this list of conditions and the following
*  disclaimer.
*
*  (2) Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
*  following disclaimer in the documentation and/or other materials provided with the distribution.
*
*  (3) Neither the name of the copyright holder nor the names of any contributors may be used to endorse or promote
*  products derived from this software without specific prior written permission from the respective party.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
*  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
*  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER, THE UNITED STATES GOVERNMENT, OR ANY CONTRIBUTORS BE LIABLE FOR
*  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
*  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
*  AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
*  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**********************************************************************************************************************/

#ifndef __easycurl_h
#define __easycurl_h

#include <wx/event.h>
#include <wx/datetime.h>
#include <wx/sstream.h>

BEGIN_DECLARE_EVENT_TYPES()
DECLARE_EVENT_TYPE(wxEASYCURL_EVENT, 7578)
END_DECLARE_EVENT_TYPES()

class wxEasyCurlEvent : public wxEvent
{
public:
	enum { STARTED, PROGRESS, FINISHED };

	wxEasyCurlEvent(int id, wxEventType type, int code, const wxString &msg = wxEmptyString, const wxString &url = wxEmptyString,
		double bytes = 0.0, double total = 0.0)
		: wxEvent(id, type) {
		m_code = code; m_msg = msg; m_dt = wxDateTime::Now(); m_url = url; m_bytes = bytes; m_total = total;
	}
	wxEasyCurlEvent(const wxEasyCurlEvent &evt)
		: wxEvent(evt.GetId(), evt.GetEventType()),
		m_code(evt.m_code),
		m_msg(evt.m_msg),
		m_dt(evt.m_dt),
		m_url(evt.m_url),
		m_bytes(evt.m_bytes),
		m_total(evt.m_total)
	{ }

	int GetStatusCode() const { return m_code; }
	wxString GetMessage() const { return m_msg; }
	virtual wxEvent* Clone() const { return new wxEasyCurlEvent(*this); }
	wxString GetUrl() const { return m_url; }
	double GetBytesTransferred() const { return m_bytes; }
	double GetBytesTotal() const { return m_total; }

protected:
	int m_code;
	wxString m_msg;
	wxDateTime m_dt;
	wxString m_url;
	double m_bytes, m_total;
};

typedef void (wxEvtHandler::*wxEasyCurlEventFunction)(wxEasyCurlEvent&);

#define wxEasyCurlEventFunction(func) \
		(wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(wxEasyCurlEventFunction, &func)

#define EVT_EASYCURL(id, fn) \
		wx__DECLARE_EVT1(wxEASYCURL_EVENT, id, wxEasyCurlEventFunction(fn))

class wxEasyCurl : public wxObject
{
public:
	// app-wide init and shutdown calls for underlying libcurl initialization
	static void Initialize();
	static void Shutdown();
	static void SetApiKeys(const wxString &google_key, const wxString &bing_key);
	static void SetUrlEscape(const wxString &key, const wxString &value);
	static void SetProxyAddress(const wxString &proxy);
	static wxString GetProxyForURL(const wxString &url);
	static wxArrayString GetProxyAutodetectMessages();

	// geocoding function using google APIs.
	// call is synchronous.  Optionally determine time
	// zone from lat/lon using second service call
	static bool GeoCode(const wxString &address,
		double *lat, double *lon, double *tz = 0);
	enum MapProvider { GOOGLE_MAPS, BING_MAPS };
	static wxBitmap StaticMap(double lat, double lon, int zoom, MapProvider service = BING_MAPS);

	wxEasyCurl(wxEvtHandler *handler = 0, int id = wxID_ANY);
	virtual ~wxEasyCurl();

	void SetPostData(const wxString &s) { m_postData = s; }
	void AddHttpHeader(const wxString &s) { m_httpHeaders.Add(s); }

	// progress reporting methods
	// send wxEasyCurlEvents to the specified wxEvtHandler, and events have the given id
	// the event handler will be called in the main thread
	void SetEventHandler(wxEvtHandler *hh, int id);

	wxString GetDataAsString();
	wxImage GetDataAsImage(wxBitmapType bittype = wxBITMAP_TYPE_JPEG);
	bool WriteDataToFile(const wxString &file);

	// asynchronous operation
	void Start(const wxString &url);
	bool Wait(bool yield = false); // must be called to finish download
	bool IsStarted();
	bool IsFinished();
	void Cancel(); // returns immediately
	bool Ok();
	wxString GetLastError();

	// synchronous operation
	bool Get(const wxString &url,
		const wxString &progress_dialog_msg = wxEmptyString,
		wxWindow *parent = NULL);

	class DLThread;
	DLThread *GetThread();
protected:
	friend class DLThread;
	DLThread *m_thread;
	wxEvtHandler *m_handler;
	int m_id;

	wxString m_postData;
	wxArrayString m_httpHeaders;
};

#endif
