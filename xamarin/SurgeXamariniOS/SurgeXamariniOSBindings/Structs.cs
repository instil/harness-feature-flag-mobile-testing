using System;

namespace SurgeXamariniOSBindings
{
    /// <summary>
    /// Defines the video encoding and associated decoder used for a RTSP stream. 
    /// </summary>
	public enum RtspSessionType {
        H264,
        MP4V,
        MJPEG,
        UNKNOWN
	}

    /// <summary>
    /// Defines all possible RTSP error codes, as defined by the RTSP specification.
    /// </summary>
    public enum RtspErrorCode {
        UnknownFailure = -1,
        Success = 200,
        Redirect = 302,
        MethodNotAllowed = 405,
        ParameterNotUnderstood = 451,
        ConferenceNotFound = 452,
        NotEnoughBandwidth = 453,
        SessionNotFound = 454,
        MethodNotValidInThisState = 455,
        HeaderFieldNotValidInThisState = 456,
        InvalidRange = 457,
        ParameterIsReadOnly = 458,
        AggregateOperationNotAllowed = 459,
        OnlyAggregationOperationAllowed = 460,
        UnsupportedTransport = 461,
        DestinationUnreachable = 462,
        OptionNotSupported = 551
    }
}

