package com.wardrumstudios.utils;

public class WarBilling extends WarBase {
    public native void notifyChange(String str, int i);
    public native void changeConnection(boolean z);

    public void AddSKU(String str)
    {
        System.out.println("**** AddSKU: " + str);
    }

    public boolean InitBilling()
    {
        System.out.println("**** InitBilling()");
        return true;
    }

    public String LocalizedPrice(String str) {
        System.out.println("**** LocalizedPrice: " + str);
        return "";
    }

    public boolean RequestPurchase(String str)
    {
        System.out.println("**** RequestPurchase: " + str);
        return true;
    }

    public void SetBillingKey(String str)
    {
        System.out.println("**** SetBillingKey: " + str);
    }
}
