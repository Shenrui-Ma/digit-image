#include "pch.h"
#include "framework.h"
#ifndef SHARED_HANDLERS
#include "exp7.h"
#endif
#include "exp7Doc.h"
#include "exp7View.h"
#include <opencv2/opencv.hpp>
#include <string>

using namespace cv;
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(Cexp7View, CView)

BEGIN_MESSAGE_MAP(Cexp7View, CView)
    ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
    ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
    ON_COMMAND(ID_FILE_PRINT_PREVIEW, &Cexp7View::OnFilePrintPreview)
    ON_WM_CONTEXTMENU()
    ON_WM_RBUTTONUP()
    ON_COMMAND(ID_FILE_OPEN, &Cexp7View::OnLoadImage)
    ON_COMMAND(ID_TRANSFORM_TRANSLATION, &Cexp7View::OnTranslation)
    ON_COMMAND(ID_TRANSFORM_ROTATION, &Cexp7View::OnRotation)
    ON_COMMAND(ID_TRANSFORM_SCALING, &Cexp7View::OnScaling)
    ON_COMMAND(ID_AFFINE_TRANSFORM, &Cexp7View::OnAffineTransform)

    //exp8
    ON_COMMAND(ID_PROCESS_BRIGHTNESS, &Cexp7View::OnAdjustBrightness)
    ON_COMMAND(ID_PROCESS_CONTRAST, &Cexp7View::OnAdjustContrast)
    ON_COMMAND(ID_PROCESS_AUTOCONTRAST, &Cexp7View::OnAutoContrast)
    ON_COMMAND(ID_PROCESS_LINEAR, &Cexp7View::OnLinearTransform)
    ON_COMMAND(ID_PROCESS_LOG, &Cexp7View::OnLogTransform)
END_MESSAGE_MAP()

Cexp7View::Cexp7View() noexcept
{
}

Cexp7View::~Cexp7View()
{
}

BOOL Cexp7View::PreCreateWindow(CREATESTRUCT& cs)
{
    // TODO: 在此处通过修改 CREATESTRUCT cs 来修改窗口类或样式
    return CView::PreCreateWindow(cs);
}

void Cexp7View::OnDraw(CDC* pDC)
{
    Cexp7Doc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);
    if (!pDoc)
        return;

    // 显示图像
    if (!m_srcImage.empty())
    {
        Mat temp;
        m_srcImage.copyTo(temp);

        // 转换为DIB格式显示
        int height = temp.rows;
        int width = temp.cols;
        uchar buffer[sizeof(BITMAPINFOHEADER) + 1024];
        BITMAPINFO* bmi = (BITMAPINFO*)buffer;
        BITMAPINFOHEADER* bmih = &(bmi->bmiHeader);

        bmih->biSize = sizeof(BITMAPINFOHEADER);
        bmih->biWidth = width;
        bmih->biHeight = -height; // 负值表示原点在左上角
        bmih->biPlanes = 1;
        bmih->biBitCount = 24;
        bmih->biCompression = BI_RGB;
        bmih->biSizeImage = 0;

        SetDIBitsToDevice(pDC->GetSafeHdc(), 0, 0, width, height,
            0, 0, 0, height, temp.data, bmi, DIB_RGB_COLORS);
    }
}

void Cexp7View::OnLoadImage()
{
    CFileDialog dlg(TRUE, _T("*.bmp"), NULL,
        OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY,
        _T("图像文件(*.bmp;*.jpg;*.png)|*.bmp;*.jpg;*.png|All Files (*.*)|*.*||"), NULL);

    if (dlg.DoModal() == IDOK)
    {
        CString pathName = dlg.GetPathName();
        // 转换 CString 到 std::string
        CT2CA pszConvertedAnsiString(pathName);
        std::string strPath(pszConvertedAnsiString);

        // 读取图像
        m_srcImage = imread(strPath);

        if (!m_srcImage.empty())
        {
            Invalidate(TRUE);
        }
        else
        {
            AfxMessageBox(_T("无法加载图像！"));
        }
    }
}

void Cexp7View::DoAffineTransform()
{
    if (m_srcImage.empty())
        return;

    Point2f srcTri[3];
    srcTri[0] = Point2f(0, 0);
    srcTri[1] = Point2f(m_srcImage.cols - 1, 0);
    srcTri[2] = Point2f(0, m_srcImage.rows - 1);

    Point2f dstTri[3];
    dstTri[0] = Point2f(0, m_srcImage.rows * 0.33f);
    dstTri[1] = Point2f(m_srcImage.cols * 0.85f, m_srcImage.rows * 0.25f);
    dstTri[2] = Point2f(m_srcImage.cols * 0.15f, m_srcImage.rows * 0.7f);

    Mat warpMat = getAffineTransform(srcTri, dstTri);
    warpAffine(m_srcImage, m_dstImage, warpMat, m_srcImage.size());

    m_srcImage = m_dstImage.clone();
    Invalidate(TRUE);
}

void Cexp7View::DoCustomAffineTransform(double angle, double scale, Point2f center)
{
    if (m_srcImage.empty())
        return;

    Mat rotMat = getRotationMatrix2D(center, angle, scale);
    warpAffine(m_srcImage, m_dstImage, rotMat, m_srcImage.size());

    m_srcImage = m_dstImage.clone();
    Invalidate(TRUE);
}

void Cexp7View::OnAffineTransform()
{
    DoAffineTransform();
}

// Cexp7View 打印


void Cexp7View::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL Cexp7View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void Cexp7View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void Cexp7View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}

void Cexp7View::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void Cexp7View::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}

// 平移变换实现
void Cexp7View::DoTranslation(int dx, int dy)
{
    if (m_srcImage.empty()) return;

    Mat trans_mat = (Mat_<double>(2, 3) << 1, 0, dx, 0, 1, dy);
    warpAffine(m_srcImage, m_dstImage, trans_mat, m_srcImage.size());
    m_srcImage = m_dstImage.clone();
    Invalidate(TRUE);
}

// 旋转变换实现
void Cexp7View::DoRotation(double angle)
{
    if (m_srcImage.empty()) return;

    Point2f center((float)m_srcImage.cols / 2, (float)m_srcImage.rows / 2);
    Mat rot_mat = getRotationMatrix2D(center, angle, 1.0);
    warpAffine(m_srcImage, m_dstImage, rot_mat, m_srcImage.size());
    m_srcImage = m_dstImage.clone();
    Invalidate(TRUE);
}

// 缩放变换实现
void Cexp7View::DoScaling(double sx, double sy)
{
    if (m_srcImage.empty()) return;

    resize(m_srcImage, m_dstImage, Size(), sx, sy, INTER_LINEAR);
    m_srcImage = m_dstImage.clone();
    Invalidate(TRUE);
}

// 菜单响应函数实现
void Cexp7View::OnTranslation()
{
    if (m_srcImage.empty())
    {
        AfxMessageBox(_T("请先加载图像！"));
        return;
    }

    // 创建对话框获取平移参数
    int dx = 50;  // 默认平移值
    int dy = 50;

    // 这里可以添加对话框来获取用户输入的 dx, dy 值

    DoTranslation(dx, dy);
}

void Cexp7View::OnRotation()
{
    if (m_srcImage.empty())
    {
        AfxMessageBox(_T("请先加载图像！"));
        return;
    }

    // 默认旋转45度
    double angle = 45.0;

    // 这里可以添加对话框来获取用户输入的角度值

    DoRotation(angle);
}

void Cexp7View::OnScaling()
{
    if (m_srcImage.empty())
    {
        AfxMessageBox(_T("请先加载图像！"));
        return;
    }

    // 默认缩放因子
    double sx = 1.5;
    double sy = 1.5;

    // 这里可以添加对话框来获取用户输入的缩放因子

    DoScaling(sx, sy);
}

// Cexp7View 诊断

#ifdef _DEBUG
void Cexp7View::AssertValid() const
{
	CView::AssertValid();
}

void Cexp7View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

Cexp7Doc* Cexp7View::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(Cexp7Doc)));
	return (Cexp7Doc*)m_pDocument;
}
#endif //_DEBUG


// Cexp7View 消息处理程序

// exp8
void Cexp7View::DoAdjustBrightness(int value)
{
    if (m_srcImage.empty()) return;

    Mat newImage = Mat::zeros(m_srcImage.size(), m_srcImage.type());
    m_srcImage.convertTo(newImage, -1, 1, value);

    m_srcImage = newImage.clone();
    Invalidate(TRUE);
}

void Cexp7View::DoAdjustContrast(double value)
{
    if (m_srcImage.empty()) return;

    Mat newImage = Mat::zeros(m_srcImage.size(), m_srcImage.type());
    m_srcImage.convertTo(newImage, -1, value, 0);

    m_srcImage = newImage.clone();
    Invalidate(TRUE);
}

void Cexp7View::DoAutoContrast()
{
    if (m_srcImage.empty()) return;

    Mat newImage;
    if (m_srcImage.channels() == 1) {
        normalize(m_srcImage, newImage, 0, 255, NORM_MINMAX);
    }
    else {
        vector<Mat> channels;
        split(m_srcImage, channels);
        for (int i = 0; i < channels.size(); i++) {
            normalize(channels[i], channels[i], 0, 255, NORM_MINMAX);
        }
        merge(channels, newImage);
    }

    m_srcImage = newImage.clone();
    Invalidate(TRUE);
}

void Cexp7View::DoLinearTransform(double a, double b)
{
    if (m_srcImage.empty()) return;

    Mat newImage = a * m_srcImage + b;
    newImage.convertTo(newImage, CV_8UC3);

    m_srcImage = newImage.clone();
    Invalidate(TRUE);
}

void Cexp7View::DoLogTransform(double c)
{
    if (m_srcImage.empty()) return;

    // 创建一个与原图像大小和类型相同的新图像
    Mat newImage = Mat::zeros(m_srcImage.size(), m_srcImage.type());

    // 转换为浮点型以进行计算
    m_srcImage.convertTo(newImage, CV_32F);

    // 对每个像素应用对数变换
    // s = c * log(1 + r)，其中r是输入像素值
    newImage = newImage / 255.0; // 归一化到[0,1]范围
    add(newImage, 1.0, newImage); // 加1防止取对数时出现负值
    log(newImage, newImage);
    newImage = c * newImage;

    // 将结果缩放到[0,255]范围
    double minVal, maxVal;
    minMaxLoc(newImage, &minVal, &maxVal);
    newImage = 255.0 * (newImage - minVal) / (maxVal - minVal);

    // 转换回8位无符号整型
    newImage.convertTo(newImage, CV_8UC3);

    m_srcImage = newImage.clone();
    Invalidate(TRUE);
}

// 消息响应函数实现
void Cexp7View::OnAdjustBrightness()
{
    if (m_srcImage.empty())
    {
        AfxMessageBox(_T("请先加载图像！"));
        return;
    }
    // 设置固定的亮度调整值
    DoAdjustBrightness(50);
}

void Cexp7View::OnAdjustContrast()
{
    if (m_srcImage.empty())
    {
        AfxMessageBox(_T("请先加载图像！"));
        return;
    }
    // 设置固定的对比度调整值
    DoAdjustContrast(1.5);
}

void Cexp7View::OnAutoContrast()
{
    if (m_srcImage.empty())
    {
        AfxMessageBox(_T("请先加载图像！"));
        return;
    }
    DoAutoContrast();
}

void Cexp7View::OnLinearTransform()
{
    if (m_srcImage.empty())
    {
        AfxMessageBox(_T("请先加载图像！"));
        return;
    }
    // 设置固定的线性变换参数
    DoLinearTransform(1.2, 30);
}

void Cexp7View::OnLogTransform()
{
    if (m_srcImage.empty())
    {
        AfxMessageBox(_T("请先加载图像！"));
        return;
    }
    // 设置固定的对数变换参数
    DoLogTransform(10);
}