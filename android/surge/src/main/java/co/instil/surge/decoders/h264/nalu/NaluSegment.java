package co.instil.surge.decoders.h264.nalu;

public class NaluSegment {
    private NaluType type;
    private int headerSize;
    private int offset;
    private byte[] buffer;

    public NaluSegment(int type, int headerSize, int offset) {
        this.type = NaluType.getTypeFromCode(type);
        this.headerSize = headerSize;
        this.offset = offset;
    }

    public int getHeaderSize() {
        return headerSize;
    }

    public int getOffset() {
        return offset;
    }

    public byte[] getBuffer() {
        return buffer;
    }

    public int getBufferSize() {
        return buffer.length;
    }

    public NaluType getType() {
        return type;
    }

    public void setOffset(int offset) {
        this.offset = offset;
    }

    public void setHeaderSize(int headerSize) {
        this.headerSize = headerSize;
    }

    public void setBuffer(byte[] buffer) throws Exception {
        if (buffer.length <= 6) {
            throw new Exception("Nalu buffer is too small to be valid.");
        }

        // Ensure nalu is in Annex-B Format of 4 magic bytes
        if (buffer[0] == 0x00 && buffer[1] == 0x00 && buffer[2] == 0x01) {
            // Ensure fixed header size
            setHeaderSize(4);
            byte[] annexb = new byte[buffer.length + 1];
            annexb[0] = 0x00;
            System.arraycopy(buffer, 0, annexb, 1, buffer.length);
            this.buffer = annexb;
        } else if (buffer[0] == 0x00 && buffer[1] == 0x00 && buffer[2] == 0x00 && buffer[3] == 0x01) {
            // nothing to do already annex-b
            this.buffer = buffer;
        } else {
            // invalid buffer
            throw new Exception("Nalu buffer contains invalid magic-byte sequence");
        }
    }

    @Override
    public String toString() {
        return String.format("\n{\n\ttype: \"%s\", \n\theaderSize: \"%s\", \n\toffset: \"%s\"\n}", type, headerSize, offset);
    }
}
