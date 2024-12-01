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
    ON_COMMAND(ID_HISTOGRAM_BIN, &Cexp7View::OnHistogramBin)
    ON_COMMAND(ID_HISTOGRAM_EQUALIZE, &Cexp7View::OnHistogramEqualize)
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

//exp9
// 添加直方图绘制函数实现
void Cexp7View::DrawHistogram(const Mat& image, Mat& histImage)
{
    // 检查图像是否为空
    if (image.empty()) return;

    // 计算直方图
    vector<Mat> bgr_planes;
    split(image, bgr_planes);

    // 创建直方图显示图像
    int hist_w = 512, hist_h = 400;
    histImage = Mat::zeros(hist_h, hist_w, CV_8UC3);

    // 设置直方图参数
    int histSize = 256;
    float range[] = { 0, 256 };
    const float* histRange = { range };

    // 根据图像通道数分别处理
    if (image.channels() == 3) {
        // 彩色图像处理
        Mat b_hist, g_hist, r_hist;

        // 确保通道数量正确
        if (bgr_planes.size() >= 3) {
            calcHist(&bgr_planes[0], 1, 0, Mat(), b_hist, 1, &histSize, &histRange);
            calcHist(&bgr_planes[1], 1, 0, Mat(), g_hist, 1, &histSize, &histRange);
            calcHist(&bgr_planes[2], 1, 0, Mat(), r_hist, 1, &histSize, &histRange);

            // 归一化直方图数据
            normalize(b_hist, b_hist, 0, hist_h, NORM_MINMAX);
            normalize(g_hist, g_hist, 0, hist_h, NORM_MINMAX);
            normalize(r_hist, r_hist, 0, hist_h, NORM_MINMAX);

            // 绘制直方图
            for (int i = 0; i < histSize; i++) {
                line(histImage,
                    Point(i * 2, hist_h),
                    Point(i * 2, hist_h - b_hist.at<float>(i)),
                    Scalar(255, 0, 0), 2);
                line(histImage,
                    Point(i * 2, hist_h),
                    Point(i * 2, hist_h - g_hist.at<float>(i)),
                    Scalar(0, 255, 0), 2);
                line(histImage,
                    Point(i * 2, hist_h),
                    Point(i * 2, hist_h - r_hist.at<float>(i)),
                    Scalar(0, 0, 255), 2);
            }
        }
    }
    else {
        // 灰度图像处理
        Mat gray_hist;
        calcHist(&image, 1, 0, Mat(), gray_hist, 1, &histSize, &histRange);
        normalize(gray_hist, gray_hist, 0, hist_h, NORM_MINMAX);

        // 绘制灰度直方图
        for (int i = 0; i < histSize; i++) {
            line(histImage,
                Point(i * 2, hist_h),
                Point(i * 2, hist_h - gray_hist.at<float>(i)),
                Scalar(255, 255, 255), 2);
        }
    }
}
// 添加直方图装箱函数实现
void Cexp7View::DoHistogramBin(int binSize)
{
    if (m_srcImage.empty()) return;

    Mat grayImage;
    cvtColor(m_srcImage, grayImage, COLOR_BGR2GRAY);

    // 执行直方图装箱
    Mat binned;
    grayImage.convertTo(binned, CV_32F);
    binned = binned / binSize;
    binned.convertTo(binned, CV_8U);
    binned = binned * binSize;

    // 转换回彩色图像以保持一致性
    cvtColor(binned, m_dstImage, COLOR_GRAY2BGR);
    m_srcImage = m_dstImage.clone();

    // 绘制处理后的直方图
    DrawHistogram(m_srcImage, m_histImage);
    Invalidate(TRUE);
}
// 添加直方图均衡化函数实现
void Cexp7View::DoHistogramEqualize()
{
    if (m_srcImage.empty()) return;

    Mat ycrcb;
    cvtColor(m_srcImage, ycrcb, COLOR_BGR2YCrCb);

    vector<Mat> channels;
    split(ycrcb, channels);

    // 对亮度通道进行直方图均衡化
    equalizeHist(channels[0], channels[0]);

    merge(channels, ycrcb);
    cvtColor(ycrcb, m_dstImage, COLOR_YCrCb2BGR);

    m_srcImage = m_dstImage.clone();

    // 绘制处理后的直方图
    DrawHistogram(m_srcImage, m_histImage);
    Invalidate(TRUE);
}

// 添加菜单响应函数实现
void Cexp7View::OnHistogramBin()
{
    if (m_srcImage.empty())
    {
        AfxMessageBox(_T("请先加载图像！"));
        return;
    }

    // 使用固定的bin大小，这里设置为32
    DoHistogramBin(32);
}

void Cexp7View::OnHistogramEqualize()
{
    if (m_srcImage.empty())
    {
        AfxMessageBox(_T("请先加载图像！"));
        return;
    }

    DoHistogramEqualize();
}

// 修改 OnDraw 函数，添加直方图显示
void Cexp7View::OnDraw(CDC* pDC)
{
    Cexp7Doc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);
    if (!pDoc)
        return;

    // 显示原图像
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
        bmih->biHeight = -height;
        bmih->biPlanes = 1;
        bmih->biBitCount = 24;
        bmih->biCompression = BI_RGB;
        bmih->biSizeImage = 0;

        SetDIBitsToDevice(pDC->GetSafeHdc(), 0, 0, width, height,
            0, 0, 0, height, temp.data, bmi, DIB_RGB_COLORS);

        // 显示直方图
        if (!m_histImage.empty())
        {
            Mat hist_temp;
            m_histImage.copyTo(hist_temp);

            bmih->biWidth = hist_temp.cols;
            bmih->biHeight = -hist_temp.rows;

            SetDIBitsToDevice(pDC->GetSafeHdc(), width + 10, 0,
                hist_temp.cols, hist_temp.rows,
                0, 0, 0, hist_temp.rows,
                hist_temp.data, bmi, DIB_RGB_COLORS);
        }
    }
}