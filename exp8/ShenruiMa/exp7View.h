
#pragma once
#include <opencv2/opencv.hpp>
using namespace cv;

class Cexp7View : public CView
{
protected:
    Cexp7View() noexcept;
    DECLARE_DYNCREATE(Cexp7View)

    // 特性
public:
    Cexp7Doc* GetDocument() const;
    //exp8
    void DoAdjustBrightness(int value);           // 调整亮度
    void DoAdjustContrast(double value);          // 调整对比度
    void DoAutoContrast();                        // 自动调整对比度
    void DoLinearTransform(double a, double b);   // 线性变换
    void DoLogTransform(double c);                // 对数变换


    // 属性
private:
    Mat m_srcImage;    // 原始图像
    Mat m_dstImage;    // 变换后图像

    // 操作
public:
    void DoTranslation(int dx, int dy);              // 平移变换
    void DoRotation(double angle);                   // 旋转变换
    void DoScaling(double sx, double sy);            // 缩放变换
    void DoAffineTransform();    // 仿射变换
    void DoCustomAffineTransform(double angle, double scale, Point2f center);

    // 重写
public:
    virtual void OnDraw(CDC* pDC);
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

protected:
    virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
    virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
    virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

    //exp8
    afx_msg void OnAdjustBrightness();
    afx_msg void OnAdjustContrast();
    afx_msg void OnAutoContrast();
    afx_msg void OnLinearTransform();
    afx_msg void OnLogTransform();

    // 实现
public:
    virtual ~Cexp7View();

#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:
    afx_msg void OnFilePrintPreview();
    afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
    afx_msg void OnLoadImage();      // 加载图像
    afx_msg void OnTranslation();
    afx_msg void OnRotation();
    afx_msg void OnScaling();
    afx_msg void OnAffineTransform(); // 仿射变换菜单响应
    DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG
inline Cexp7Doc* Cexp7View::GetDocument() const
{
    return reinterpret_cast<Cexp7Doc*>(m_pDocument);
}
#endif