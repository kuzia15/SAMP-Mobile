package com.nvidia.devtech;

import java.io.InputStream;

public class NvAPKFile
{
    /** The actual data bytes. */
    public byte[] data;
    public int bufferSize;
    /** The length of the data. */
    public int length;
    /** The length of the data. */
    public int position;
    /** Stream used to push/pull data. */
    public InputStream is;
}
