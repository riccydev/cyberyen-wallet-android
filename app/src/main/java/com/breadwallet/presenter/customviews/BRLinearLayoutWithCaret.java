package com.breadwallet.presenter.customviews;

import android.content.Context;
import android.content.res.Resources;
import android.content.res.TypedArray;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.LinearGradient;
import android.graphics.Paint;
import android.graphics.Path;
import android.graphics.Shader;
import android.support.annotation.ColorInt;
import android.util.AttributeSet;
import android.util.Log;
import android.util.TypedValue;
import android.widget.LinearLayout;
import android.widget.RelativeLayout;

import com.breadwallet.R;
import com.breadwallet.tools.manager.TypefacesManager;
import com.breadwallet.tools.util.Utils;

import static android.R.attr.background;
import static android.R.attr.paddingLeft;
import static android.R.attr.paddingRight;
import static android.R.attr.paddingTop;
import static com.breadwallet.R.attr.customFont;

/**
 * BreadWallet
 * <p/>
 * Created by Mihail Gutan on <mihail@breadwallet.com> 2/24/17.
 * Copyright (c) 2017 breadwallet LLC
 * <p/>
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * <p/>
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * <p/>
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
public class BRLinearLayoutWithCaret extends LinearLayout {
    public static final String TAG = BRLinearLayoutWithCaret.class.getName();

    private float mXfract = 0f;
    private float mYfract = 0f;

    private Paint paint;
    private Paint backgroundPaint;
    private Path path;
    private int caretHeight;

    private int width;
    private int height;
    private boolean created;

    public BRLinearLayoutWithCaret(Context context) {
        super(context);
        init(null);
    }

    public BRLinearLayoutWithCaret(Context context, AttributeSet attrs) {
        super(context, attrs);
        init(attrs);
    }

    public BRLinearLayoutWithCaret(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        init(attrs);
    }

    public void setYFraction(final float fraction) {
        mYfract = fraction;
        float translationY = getHeight() * fraction;
        setTranslationY(translationY);
    }

    public float getYFraction() {
        return mYfract;
    }

    public void setXFraction(final float fraction) {
        mXfract = fraction;
        float translationX = getWidth() * fraction;
        setTranslationX(translationX);
    }

    public float getXFraction() {
        return mXfract;
    }

    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        super.onMeasure(widthMeasureSpec, heightMeasureSpec);
        int width = MeasureSpec.getSize(widthMeasureSpec);
        int height = MeasureSpec.getSize(heightMeasureSpec);
        // Correct any translations set before the measure was set
        setTranslationX(mXfract * width);
        setTranslationY(mYfract * height);
    }

    private void init(AttributeSet attrs) {
        paint = new Paint(Paint.ANTI_ALIAS_FLAG);
        paint.setStyle(Paint.Style.STROKE);
        float px = TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_DIP, 1, getResources().getDisplayMetrics());
        paint.setStrokeWidth(px);
        paint.setColor(getContext().getColor(R.color.separator_gray));
        path = new Path();

        backgroundPaint = new Paint(Paint.ANTI_ALIAS_FLAG);
        backgroundPaint.setStyle(Paint.Style.FILL);
        backgroundPaint.setColor(getContext().getColor(R.color.red));

        if (attrs != null) {
            TypedValue typedValue = new TypedValue();
            Resources.Theme theme = getContext().getTheme();
            theme.resolveAttribute(R.attr.background, typedValue, true);
            @ColorInt int color = typedValue.data;

            backgroundPaint.setColor(color);
        } else {
            Log.e(TAG, "init: attr is null");
        }
        setBackgroundColor(getContext().getColor(android.R.color.transparent));

    }

    @Override
    protected void onSizeChanged(int w, int h, int oldw, int oldh) {
        super.onSizeChanged(w, h, oldw, oldh);
        if (w != 0 && !created) {
            created = true;
            width = w;
            height = h;

            caretHeight = h / 10;
            int caretWidth = caretHeight * 2;

            path.moveTo(0, caretHeight);
            path.lineTo(width / 2 - caretWidth / 2, caretHeight);
            path.lineTo(width / 2, 0);
            path.lineTo(width / 2 + caretWidth / 2, caretHeight);
            path.lineTo(width, caretHeight);

            invalidate();
        }
    }


    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);
        canvas.drawRect(0, caretHeight, width, height, backgroundPaint);
        canvas.drawPath(path, paint);
    }
}