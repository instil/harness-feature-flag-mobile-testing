from libcpp cimport bool
from libcpp.string cimport string
from libcpp.vector cimport vector



cdef extern from "RtspClient.h" namespace "Surge":
    cppclass RtspClient:
        RtspClient()
        DescribeResponse * Describe(const string url, bool requires_auth, const string user, const string password)
        SetupResponse * Setup(const SessionDescription sessionDescription, bool serverAllowsAggregate)
        RtspResponse * Play(bool waitForResponse)
        RtspResponse * Pause()
        void StopClient()

