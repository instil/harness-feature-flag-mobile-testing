package co.instil.surge.decoders.h265.nalu

class H265NaluSegment(val type: H265NaluType, bytes: ByteArray) {
    private val magicHeaderLength: Int
    val payload: ByteArray

    val payloadLength: Int
        get() = payload.size

    init {
        magicHeaderLength = if (isParameterSet()) 4 else 3
        payload = ByteArray(bytes.size + magicHeaderLength)

        for (i in 0..(magicHeaderLength - 2)) {
            payload[i] = 0x00
        }
        payload[magicHeaderLength - 1] = 0x01
        System.arraycopy(bytes, 0, payload, magicHeaderLength, bytes.size)
    }

    private fun isParameterSet() =
        type == H265NaluType.VPS || type == H265NaluType.PPS || type == H265NaluType.SPS

    override fun toString(): String =
        String.format("\n{\n\ttype: \"%s\", \n\tpayload length: \"%s\"}", type, payload.size)
}
