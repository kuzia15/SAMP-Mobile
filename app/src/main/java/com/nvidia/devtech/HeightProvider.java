package com.nvidia.devtech;

import android.app.Activity;
import android.graphics.Rect;
import android.graphics.drawable.ColorDrawable;
import android.view.Gravity;
import android.view.View;
import android.view.ViewTreeObserver;
import android.view.WindowManager;
import android.widget.LinearLayout;
import android.widget.PopupWindow;

public class HeightProvider extends PopupWindow implements ViewTreeObserver.OnGlobalLayoutListener {
    private Activity mActivity;
    private View rootView;
    private HeightListener listener;
    private int heightMaxHorizontal; // Record the maximum height of the pop content area
    private int heightMaxVertical; // Record the maximum height of the pop content area

    public HeightProvider(final Activity activity) {
        super(activity);
        this.mActivity = activity;

        // Basic configuration
        rootView = new View(activity);
        setContentView(rootView);

        // Monitor global Layout changes
        rootView.getViewTreeObserver().addOnGlobalLayoutListener(this);
        setBackgroundDrawable(new ColorDrawable(0));

        // Set width to 0 and height to full screen
        setWidth(0);
        setHeight(LinearLayout.LayoutParams.MATCH_PARENT);

        // Set keyboard pop-up mode
        setSoftInputMode(WindowManager.LayoutParams.SOFT_INPUT_ADJUST_RESIZE);
        setInputMethodMode(PopupWindow.INPUT_METHOD_NEEDED);
    }

    public HeightProvider init(View view) {
        if (!isShowing()) {
            // Delay loading popupwindow, if not, error will be reported
            final View finalView = view;
            view.post(new Runnable() {
                @Override
                public void run() {
                    showAtLocation(finalView, Gravity.NO_GRAVITY, 0, 0);
                }
            });
        }
        return this;
    }



    public HeightProvider setHeightListener(HeightListener listener) {
        this.listener = listener;
        return this;
    }

    @Override
    public void onGlobalLayout() {
        Rect rect = new Rect();
        rootView.getWindowVisibleDisplayFrame(rect);
        int keyboardHeight = 0;
        if(rect.bottom > rect.right)
        {
            if (rect.bottom > heightMaxVertical) {
                heightMaxVertical = rect.bottom;
            }
            // The difference between the two is the height of the keyboard
            keyboardHeight = heightMaxVertical - rect.bottom;
        }
        else
        {
            if (rect.bottom > heightMaxHorizontal) {
                heightMaxHorizontal = rect.bottom;
            }
            // The difference between the two is the height of the keyboard
            keyboardHeight = heightMaxHorizontal - rect.bottom;
        }

        if (listener != null) {
            listener.onHeightChanged(mActivity.getResources().getConfiguration().orientation, keyboardHeight);
        }
    }

    public interface HeightListener {
        void onHeightChanged(int orientation, int height);
    }
}