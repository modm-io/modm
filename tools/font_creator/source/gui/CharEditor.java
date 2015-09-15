/*
 * Erstellt am: 05.12.2003
 * Autor:       Maximilian Thiele
 * 
 */

package gui;

import gui.util.EditorPopupMenu;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Point;
import java.awt.Rectangle;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;
import java.awt.geom.AffineTransform;
import java.awt.image.AffineTransformOp;
import java.awt.image.BufferedImage;
import java.awt.image.IndexColorModel;
import java.awt.image.Raster;

import javax.swing.Action;
import javax.swing.ActionMap;
import javax.swing.JComponent;
import javax.swing.TransferHandler;

import util.FontCharacter;
import util.IndexColorModelUtils;

public class CharEditor extends JComponent implements MouseListener,
		MouseMotionListener {
	private static final long serialVersionUID = 1001182661640895939L;
	public static final int TOOL_NONE = 1;
	public static final int TOOL_FREEHAND = 2;
	public static final int TOOL_STRAIGHT_LINES = 3;
	public static final int TOOL_DRAW_RECT = 4;
	public static final int TOOL_FILL_RECT = 5;
	public static final int TOOL_DRAW_OVAL = 6;
	public static final int TOOL_FILL_OVAL = 7;
	public static final int TOOL_EYEDROPPER = 8;
	
	private int zoom;
	private int xO = 0;
	private int yO = 0;
	
	@SuppressWarnings("unused")
	private FontEditor parent;
	
	private EditorPopupMenu popup;
	private BufferedImage drawMask;
	private Point mouseDown;
	private IndexColorModel cm;

	private int color;
	private int tool;
	
	private FontCharacter fc;
	private MainWindow mainWnd;

	public CharEditor(FontCharacter fc, FontEditor parent, IndexColorModel cm,
			MainWindow mainWnd) {
		this.fc = fc;
		this.parent = parent;
		this.cm = cm;
		this.mainWnd = mainWnd;
		zoom = 30;
		popup = new EditorPopupMenu(this);

		color = 0;
		tool = TOOL_FREEHAND;

		addMouseListener(this);
		addMouseMotionListener(this);

		setFocusable(true);
		ActionMap map = this.getActionMap();
		map.put(TransferHandler.getCopyAction().getValue(Action.NAME),
				TransferHandler.getCopyAction());
		map.put(TransferHandler.getPasteAction().getValue(Action.NAME),
				TransferHandler.getPasteAction());
	}

	public void clear() {
		Graphics2D g = fc.getImage().createGraphics();
		g.setColor(Color.WHITE);
		g.fillRect(0, 0, fc.getWidth(), fc.getHeight());
	}

	public void setChar(FontCharacter fc) {
		this.fc = fc;
		repaint();
	}

	public void paint(Graphics graphics)
	{		
		if (fc.getImage() != null)
		{
			Graphics2D g = (Graphics2D) graphics;
			int w = fc.getWidth();
			int h = fc.getHeight();
			xO = 0;
			yO = 0;
			if (w * zoom < getWidth())
				xO = (getWidth() - w * zoom) / 2;
			if (w * zoom < getHeight())
				yO = (getHeight() - h * zoom) / 2;
			int x = 0;
			int y = 0;
			
			// Pixel
			Raster imageData = fc.getImage().getData();
			int[] iArray = null;
			int[] a = imageData.getPixels(0, 0, w, h, iArray);
			for (int i = 0; i < a.length; i++) {
				g.setColor(IndexColorModelUtils.getColorForIndex(cm, a[i]));

				if (i != 0 && i % w == 0) {
					x = 0;
					y += zoom;
				}

				g.fillRect(x + xO, y + yO, zoom, zoom);
				x += zoom;
			}
			
			// draw Mask
			if (drawMask != null && drawMask.getWidth() == fc.getWidth()
					&& drawMask.getHeight() == fc.getHeight()) {
				x = 0;
				y = 0;
				imageData = drawMask.getData();
				a = imageData.getPixels(0, 0, w, h, iArray);
				for (int i = 0; i < a.length; i++) {
					if (i != 0 && i % w == 0) {
						x = 0;
						y += zoom;
					}
					if (a[i] == 1) {
						g.setColor(new Color(120, 170, 200, 150));
						g.fillRect(x + xO, y + yO, zoom, zoom);
					}
					x += zoom;
				}
			}
			
			// Grid
			if (zoom > 4) {
				g.setColor(new Color(150, 150, 150));
				for (int i = 0; i <= w; i++) {
					g.drawLine(i * zoom + xO, 0 + yO, i * zoom + xO, h * zoom
							+ yO);
				}
				for (int i = 0; i <= h; i++) {
					g.drawLine(0 + xO, i * zoom + yO, w * zoom + xO, i * zoom
							+ yO);
				}
			}
		}
	}

	public void mouseClicked(MouseEvent e) {
		if (e.getButton() == MouseEvent.BUTTON3) {
			popup.show(this, e.getX(), e.getY());
		}
	}

	public void mouseEntered(MouseEvent e) {
	// leer
	}

	public void mouseExited(MouseEvent e) {
	// leer
	}

	public void mousePressed(MouseEvent e) {
		requestFocusInWindow();

		int w = fc.getWidth();
		int h = fc.getHeight();
		int x = (e.getX() - xO) / zoom;
		int y = (e.getY() - yO) / zoom;
		
		if (e.getButton() == MouseEvent.BUTTON1 &&
				x >= 0 && x < w &&
				y >= 0 && y < h &&
				tool != TOOL_NONE)
		{
			Graphics g;
			switch (tool) {
			case TOOL_FREEHAND:
				g = fc.getImage().createGraphics();
				g.setColor(IndexColorModelUtils.getColorForIndex(cm, color));
				g.drawLine(x, y, x, y);
				break;
			case TOOL_STRAIGHT_LINES:
				drawMask = new BufferedImage(w, h,
						BufferedImage.TYPE_BYTE_BINARY);
				g = drawMask.createGraphics();
				g.setColor(Color.WHITE);
				g.drawLine(x, y, x, y);
				break;
			case TOOL_DRAW_RECT:
				drawMask = new BufferedImage(w, h,
						BufferedImage.TYPE_BYTE_BINARY);
				g = drawMask.createGraphics();
				g.setColor(Color.WHITE);
				g.drawLine(x, y, x, y);
				break;
			case TOOL_FILL_RECT:
				drawMask = new BufferedImage(w, h,
						BufferedImage.TYPE_BYTE_BINARY);
				g = drawMask.createGraphics();
				g.setColor(Color.WHITE);
				g.drawLine(x, y, x, y);
				break;
			case TOOL_DRAW_OVAL:
				drawMask = new BufferedImage(w, h,
						BufferedImage.TYPE_BYTE_BINARY);
				g = drawMask.createGraphics();
				g.setColor(Color.WHITE);
				g.drawLine(x, y, x, y);
				break;
			case TOOL_FILL_OVAL:
				drawMask = new BufferedImage(w, h,
						BufferedImage.TYPE_BYTE_BINARY);
				g = drawMask.createGraphics();
				g.setColor(Color.WHITE);
				g.drawLine(x, y, x, y);
				break;
			}
			mouseDown = new Point(x, y);
			repaint();
		}
	}

	private Rectangle swapCoords(int x, int y, int w, int h) {
		Point[] corner = new Point[4];
		corner[0] = new Point(x, y); // upper Left corner
		corner[1] = new Point(x, y + h); // lower Left corner
		corner[2] = new Point(x + w, y); // upper Right corner
		corner[3] = new Point(x + w, y + h); // lower Right corner

		int xt = corner[0].x;
		int yt = corner[0].y;
		for (int i = 1; i < 4; i++) {
			if (corner[i].x <= xt && corner[i].y <= yt) {
				xt = corner[i].x;
				yt = corner[i].y;
			}
		}

		int xb = corner[0].x;
		int yb = corner[0].y;
		for (int i = 1; i < 4; i++) {
			if (corner[i].x >= xb && corner[i].y >= yb) {
				xb = corner[i].x;
				yb = corner[i].y;
			}
		}

		Rectangle rect = new Rectangle(xt, yt, xb - xt + 1, yb - yt + 1);
		return rect;
	}

	/*
	 * die Java eigene Funktion ist doof :)
	 */
	private void drawRect(int x, int y, int w, int h, Graphics g) {
		g.drawLine(x, y, x + w, y); // oben
		g.drawLine(x, y, x, y + h); // links
		g.drawLine(x, y + h, x + w, y + h); // unten
		g.drawLine(x + w, y, x + w, y + h); // rechts
	}

	private void fillRect(int x, int y, int w, int h, Graphics g) {
		Rectangle rect = swapCoords(x, y, w, h);
		g.fillRect(rect.x, rect.y, rect.width, rect.height);
	}

	private void drawOval(int x, int y, int w, int h, Graphics g) {
		Rectangle rect = swapCoords(x, y, w, h);
		g.drawOval(rect.x, rect.y, rect.width, rect.height);
	}

	private void fillOval(int x, int y, int w, int h, Graphics g) {
		Rectangle rect = swapCoords(x, y, w, h);
		g.fillOval(rect.x, rect.y, rect.width, rect.height);
	}

	public void mouseReleased(MouseEvent e) {
		if (e.getButton() == MouseEvent.BUTTON1 && tool != TOOL_NONE) {
			int x1 = (e.getX() - xO) / zoom;
			int y1 = (e.getY() - yO) / zoom;

			int x2, y2;
			if (mouseDown != null) {
				x2 = mouseDown.x;
				y2 = mouseDown.y;
			} else {
				x2 = 0;
				y2 = 0;
			}

			Graphics g = fc.getImage().createGraphics();
			g.setColor(IndexColorModelUtils.getColorForIndex(cm, color));

			if (y2 > y1) {
				int xt = x2;
				int yt = y2;
				x2 = x1;
				y2 = y1;
				x1 = xt;
				y1 = yt;
			}

			switch (tool) {
			case TOOL_FREEHAND:
				g.drawLine(x2, y2, x1, y1);
				break;
			case TOOL_STRAIGHT_LINES:
				g.drawLine(x2, y2, x1, y1);
				break;
			case TOOL_DRAW_RECT:
				drawRect(x2, y2, x1 - x2, y1 - y2, g);
				break;
			case TOOL_FILL_RECT:
				fillRect(x2, y2, x1 - x2, y1 - y2, g);
				break;
			case TOOL_DRAW_OVAL:
				drawOval(x2, y2, x1 - x2, y1 - y2, g);
				break;
			case TOOL_FILL_OVAL:
				fillOval(x2, y2, x1 - x2, y1 - y2, g);
				break;
			}
			mouseDown = null;
			drawMask = null;
			repaint();
		}
	}

	public void mouseDragged(MouseEvent e) {
		if (mouseDown != null) {
			int w = fc.getWidth();
			int h = fc.getHeight();

			int x1 = (e.getX() - xO) / zoom;
			int y1 = (e.getY() - yO) / zoom;

			int x2 = mouseDown.x;
			int y2 = mouseDown.y;

			if (y2 > y1) {
				int x = x2;
				int y = y2;
				x2 = x1;
				y2 = y1;
				x1 = x;
				y1 = y;
			}
			if (tool == TOOL_NONE || tool == TOOL_FREEHAND) {
				mainWnd.setStatusText("(" + x1 + "|" + y1 + ")");
			} else {
				mainWnd.setStatusText("(" + mouseDown.x + "|" + mouseDown.y
						+ ")-(" + x1 + "|" + y1 + ")" + " W:"
						+ (Math.abs(x1 - mouseDown.x) + 1) + " H:"
						+ (Math.abs(y1 - mouseDown.y) + 1));
			}

			Graphics g;
			switch (tool) {
			case TOOL_FREEHAND:
				g = fc.getImage().createGraphics();
				g.setColor(IndexColorModelUtils.getColorForIndex(cm, color));
				g.drawLine(x2, y2, x1, y1);
				mouseDown.x = (e.getX() - xO) / zoom;
				mouseDown.y = (e.getY() - yO) / zoom;
				break;
			case TOOL_STRAIGHT_LINES:
				g = drawMask.createGraphics();
				g.setColor(Color.BLACK);
				g.fillRect(0, 0, w, h);
				g.setColor(Color.WHITE);
				g.drawLine(x2, y2, x1, y1);
				break;
			case TOOL_DRAW_RECT:
				g = drawMask.createGraphics();
				g.setColor(Color.BLACK);
				g.fillRect(0, 0, w, h);
				g.setColor(Color.WHITE);
				drawRect(x2, y2, x1 - x2, y1 - y2, g);
				break;
			case TOOL_FILL_RECT:
				g = drawMask.createGraphics();
				g.setColor(Color.BLACK);
				g.fillRect(0, 0, w, h);
				g.setColor(Color.WHITE);
				fillRect(x2, y2, x1 - x2, y1 - y2, g);
				break;
			case TOOL_DRAW_OVAL:
				g = drawMask.createGraphics();
				g.setColor(Color.BLACK);
				g.fillRect(0, 0, w, h);
				g.setColor(Color.WHITE);
				drawOval(x2, y2, x1 - x2, y1 - y2, g);
				break;
			case TOOL_FILL_OVAL:
				g = drawMask.createGraphics();
				g.setColor(Color.BLACK);
				g.fillRect(0, 0, w, h);
				g.setColor(Color.WHITE);
				fillOval(x2, y2, x1 - x2, y1 - y2, g);
				break;
			}
			repaint();
		}
	}

	public void mouseMoved(MouseEvent e) {
		int w = fc.getWidth();
		int h = fc.getHeight();
		int x1 = (e.getX() - xO) / zoom;
		int y1 = (e.getY() - yO) / zoom;
		if (x1 < 0 || x1 >= w) {
			x1 = 0;
			y1 = 0;
		}
		if (y1 < 0 || y1 >= h) {
			x1 = 0;
			y1 = 0;
		}
		mainWnd.setStatusText("(" + x1 + "|" + y1 + ")");
	}

	public int getZoom() {
		return zoom;
	}

	public void setZoom(int zoomValue) {
		this.setPreferredSize(
				new Dimension(
						fc.getWidth() * zoom,
						fc.getHeight() * zoom));
		zoom = zoomValue;
	}

	public int getColor() {
		return color;
	}

	public void setColor(int newtool) {
		color = newtool;
	}

	public int getTool() {
		return tool;
	}

	public void setTool(int newtool) {
		tool = newtool;
	}

	public IndexColorModel getEditorColorModel() {
		return cm;
	}

	public void pasteImage(BufferedImage image) {
		Graphics2D g = fc.getImage().createGraphics();
		AffineTransformOp ao = new AffineTransformOp(new AffineTransform(),
				AffineTransformOp.TYPE_NEAREST_NEIGHBOR);
		g.drawImage(image, ao, 0, 0);
		repaint();
	}
}