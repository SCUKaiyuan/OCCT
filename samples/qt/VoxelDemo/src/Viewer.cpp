#include "Viewer.h"
#include "Timer.h"

#include <QApplication.h>
#include <QCursor.h>
#include <QMessagebox.h>

#include <QMouseEvent>

#include <WNT_Window.hxx>
#include <Graphic3d_WNTGraphicDevice.hxx>

#include <Voxel_Prs.hxx>
#include <AIS_ListOfInteractive.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>

static Handle(Graphic3d_WNTGraphicDevice) device;

Viewer::Viewer(QWidget* parent):QWidget(parent)
{
    if(device.IsNull())
    {
        device = new Graphic3d_WNTGraphicDevice();
        if (!device->GraphicDriver().IsNull())
        {
            myGraphicDriver = Handle(OpenGl_GraphicDriver)::DownCast(device->GraphicDriver());
        }
    }
    
	Handle(V3d_Viewer) aViewer = new V3d_Viewer(device, TCollection_ExtendedString("Visu3D").ToExtString(), "",
                                                1000, V3d_XposYnegZpos,
                                                Quantity_NOC_GRAY30, V3d_ZBUFFER, V3d_GOURAUD, V3d_WAIT,
                                                true, true, V3d_TEX_NONE);
    aViewer->Init();

	aViewer->SetDefaultBackgroundColor(Quantity_NOC_BLACK);

    myView = aViewer->CreateView();
    myIC = new AIS_InteractiveContext(aViewer);
    myIC->SetDeviationCoefficient(1.e-3);

    int       windowHandle = (int) winId();
    short     hi, lo;

    lo = (short) windowHandle;
    hi = (short) (windowHandle >> 16);
    Handle(WNT_Window) hWnd =
        new WNT_Window(Handle(Graphic3d_WNTGraphicDevice)::DownCast(aViewer->Device()), (int) hi, (int) lo);

    myView->SetWindow(hWnd);
    if(!hWnd->IsMapped())
        hWnd->Map();

    myView->MustBeResized();
    myView->SetSurfaceDetail(V3d_TEX_NONE);
    myView->SetTransparency(Standard_True);
    myView->SetSize(10000.0);
    myView->SetZSize(10000.0);
    myView->SetViewMappingDefault();

    myZoom = false;
    myPan = false;
    myRotate = false;
    setMouseTracking(true);

    setMinimumSize(400, 200);
    
    myView->ZBufferTriedronSetup();
    myView->TriedronDisplay(Aspect_TOTP_LEFT_LOWER, Quantity_NOC_BLACK, 0.1, V3d_ZBUFFER);

    mySelector.Init(myView);

	setBackgroundRole( QPalette::NoRole );//NoBackground );
	// set focus policy to threat QContextMenuEvent from keyboard  
	setFocusPolicy( Qt::StrongFocus );
	setAttribute( Qt::WA_PaintOnScreen );
	setAttribute( Qt::WA_NoSystemBackground );
}

Viewer::~Viewer()
{

}

void Viewer::paintEvent(QPaintEvent * pEvent)
{
    if (!myView.IsNull())
        myView->Redraw();
}


/*!
  Get paint engine for the OpenGL viewer. [ virtual public ]
*/
QPaintEngine* Viewer::paintEngine() const
{
  return 0;
}

void Viewer::resizeEvent(QResizeEvent * e)
{
    if (!myView.IsNull())
    {
        myView->MustBeResized();
    }
}

void Viewer::mousePressEvent(QMouseEvent * mpEvent)
{
    // Memorize start point
    myStartPnt.setX(mpEvent->x());
    myStartPnt.setY(mpEvent->y());
    
    // Inform IC that the mouse cursor is at the point
    myIC->MoveTo(myStartPnt.x(), myStartPnt.y(), myView);

    // In case of rotation, define the start rotation point
    if ((mpEvent->modifiers() & Qt::ControlModifier) && (mpEvent->buttons() & Qt::RightButton))
    {
        myView->StartRotation(myStartPnt.x(), myStartPnt.y());
    }

    // Start degenerate mode
    setDegenerateMode(true);

    emit mousePressed(mpEvent->modifiers(), mpEvent->x(), mpEvent->y());
}

void Viewer::mouseMoveEvent(QMouseEvent * mmEvent)
{
    QPoint currentPnt(mmEvent->x(), mmEvent->y());

    if (mmEvent->modifiers() & Qt::ControlModifier)
    {
        if (mmEvent->buttons() & Qt::LeftButton)
        {
            myView->Zoom(myStartPnt.x(), myStartPnt.y(), currentPnt.x(), currentPnt.y());
            myStartPnt = currentPnt;
        }
        else if (mmEvent->buttons() & Qt::MidButton)
        {
            myView->Pan(currentPnt.x() - myStartPnt.x(), myStartPnt.y() - currentPnt.y());
            myStartPnt = currentPnt;
        }
        else if (mmEvent->buttons() & Qt::RightButton)
        {
            myView->Rotation(currentPnt.x(), currentPnt.y());
        }
    }
    else
    {
        myIC->MoveTo(currentPnt.x(), currentPnt.y(), myView);
    }

    emit mouseMoved(mmEvent->modifiers(), currentPnt.x(), currentPnt.y());
}

void Viewer::mouseReleaseEvent(QMouseEvent * mrEvent)
{
    if(mrEvent->button() == Qt::LeftButton)
    {
        if(!myZoom && !myPan && !myRotate)
        {
            if(mrEvent->modifiers() & Qt::ShiftModifier)
                myIC->ShiftSelect();
            else
                myIC->Select();

            // Select a voxel
            int ix = -1, iy = -1, iz = -1;
            bool detected = mySelector.Detect(mrEvent->x(), mrEvent->y(), ix, iy, iz);
            if (detected)
            {
                cout<<"("<<ix<<", "<<iy<<", "<<iz<<")"<<endl;
            }
            if (!myPrs.IsNull())
                myPrs->Highlight(ix, iy, iz);
        }
    }
    else if(mrEvent->button() == Qt::RightButton)
    {
        // Popup menu:

    
	}

    // Finish degenerate mode
    setDegenerateMode(false);
    
    emit mouseReleased(mrEvent->modifiers(), mrEvent->x(), mrEvent->y());
}

void Viewer::mouseDoubleClickEvent(QMouseEvent * mdcEvent)
{
    emit mouseDoubleClick(mdcEvent->modifiers(), mdcEvent->x(), mdcEvent->y());
}

void Viewer::setDegenerateMode(const bool on)
{
    AIS_ListOfInteractive displayed;
    myIC->DisplayedObjects(displayed);
    AIS_ListIteratorOfListOfInteractive itri(displayed);
    for (; itri.More(); itri.Next())
    {
        if (itri.Value()->DynamicType() == STANDARD_TYPE(Voxel_Prs))
        {
            Handle(Voxel_Prs) prs = Handle(Voxel_Prs)::DownCast(itri.Value());
            prs->SetDegenerateMode(on);
            myView->Redraw();
            break;
        }
    }
}
