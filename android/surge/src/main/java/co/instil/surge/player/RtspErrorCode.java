/*
 * Copyright (c) 2017 Instil Software.
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE.txt', which is part of this source code package.
 */

package co.instil.surge.player;

/**
 *
 */
public enum RtspErrorCode {
    UNKNOWN_FAILURE(-1),
    SUCCESS(200),
    REDIRECT(302),
    METHOD_NOT_ALLOWED(405),
    PARAMETER_NOT_UNDERSTOOD(451),
    CONFERENCE_NOT_FOUND(452),
    NOT_ENOUGH_BANDWIDTH(453),
    SESSION_NOT_FOUND(454),
    METHOD_NOT_VALID_IN_THIS_STATE(455),
    HEAER_FIELD_NOT_VALID_IN_THIS_STATE(456),
    INVALID_RANGE(457),
    PARAMETER_IS_READ_ONLY(458),
    AGGREGATE_OPERATION_NOT_FOUND(459),
    ONLY_AGGREGATION_OPERATION_ALLOWED(460),
    UNSUPPORTED_TRANSPORT(461),
    DESTINATION_UNREACHABLE(462);

    private int errorCode;

    RtspErrorCode(int errorCode) {
        this.errorCode = errorCode;
    }

    public int getErrorCode() {
        return errorCode;
    }

    public static RtspErrorCode valueOf(int rawErrorCode) {
        for (RtspErrorCode code : RtspErrorCode.values()) {
            if (code.getErrorCode() == rawErrorCode) {
                return code;
            }
        }
        return null;
    }
}
