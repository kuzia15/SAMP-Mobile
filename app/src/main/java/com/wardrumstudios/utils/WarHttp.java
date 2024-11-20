package com.wardrumstudios.utils;

public class WarHttp {
    protected WarHttp(WarBase warBase)
    {
        System.out.println("**** WarHttp::Init");
    }

    public String HttpGet(String str)
    {
        System.out.println("**** HttpGet");
        return "";
    }

    public byte[] HttpGetData(String url) {
        System.out.println("**** HttpGetData");
        return null;
    }

    public String HttpPost(String url) {
        System.out.println("**** HttpPost");
        return "";
    }

    public void AddHttpGetLineFeeds(boolean value) {
        System.out.println("**** AddHttpGetLineFeeds");
    }
}
