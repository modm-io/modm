/*
 * Created on 11.02.2004
 *
 * To change the template for this generated file go to
 * Window&gt;Preferences&gt;Java&gt;Code Generation&gt;Code and Comments
 */
package util;

import java.awt.Color;
import java.awt.Graphics2D;
import java.awt.Rectangle;
import java.awt.font.FontRenderContext;
import java.awt.geom.AffineTransform;
import java.awt.image.AffineTransformOp;
import java.awt.image.BufferedImage;
import java.awt.image.IndexColorModel;
import java.awt.image.Raster;

/**
 * @author Fabian Maximilian Thiele
 */
public class FontCharacter {
	private int width;
	private int height;
	private String comment;
	private Font font;

	private BufferedImage image;

	public FontCharacter(int width, int height, Font font) {
		this.width = width;
		this.height = height;
		this.font = font;
		this.comment = "";
		
		if (width > 0) {
			image = new BufferedImage(width, height,
					BufferedImage.TYPE_BYTE_INDEXED, font.getEditorColorModel());
			Graphics2D g = image.createGraphics();
			g.setColor(Color.white);
			g.fillRect(0, 0, width, height);
		}
	}

	private boolean isRowEmpty(int row) {
		if (image != null) {
			IndexColorModel cm = font.getEditorColorModel();
			Raster data = image.getData();
			int[] tmp = null;
			int[] a = data.getPixels(0, 0, image.getWidth(), image.getHeight(),
					tmp);
			int offset = row * image.getWidth();
			for (int i = offset; i < offset + image.getWidth(); i++) {
				if (cm.getRed(a[i]) != 255 && cm.getGreen(a[i]) != 255
						&& cm.getBlue(a[i]) != 255)
					return false;
			}
		}
		return true;
	}

	private boolean isColEmpty(int col) {
		if (image != null) {
			IndexColorModel cm = font.getEditorColorModel();
			Raster data = image.getData();
			int[] tmp = null;
			int[] a = data.getPixels(0, 0, image.getWidth(), image.getHeight(),
					tmp);
			int offset = col;
			for (int i = offset; i < offset
					+ (image.getWidth() * (image.getHeight() - 1)); i += image.getWidth()) {
				if (cm.getRed(a[i]) != 255 && cm.getGreen(a[i]) != 255
						&& cm.getBlue(a[i]) != 255)
					return false;
			}
		}
		return true;
	}

	public int getWidth() {
		return width;
	}

	public int getHeight() {
		return height;
	}

	public void setSize(int top, int bottom, int left, int right) {
		width += left + right;
		height += top + bottom;
		if (width < 0)
			width = 0;
		if (height < 0)
			height = 0;
		IndexColorModel cm = font.getEditorColorModel();

		if (width > 0) {
			BufferedImage img = new BufferedImage(width, height,
					BufferedImage.TYPE_BYTE_INDEXED, cm);
			Graphics2D g = img.createGraphics();
			g.setColor(Color.WHITE);
			g.fillRect(0, 0, width, height);
			if (image != null) {
				AffineTransformOp ao = new AffineTransformOp(
						new AffineTransform(),
						AffineTransformOp.TYPE_NEAREST_NEIGHBOR);
				g.drawImage(image, ao, left, top);
			}
			image = img;
		} else {
			image = null;
		}
	}

	public Rectangle importChar(java.awt.Font fontType, char ascii) {
		int y = (int) (fontType.getSize() * 1.5);
		Graphics2D g = image.createGraphics();
		g.setFont(fontType);

		FontRenderContext frc = g.getFontRenderContext();
		Rectangle rect = fontType.getStringBounds("" + ascii, frc).getBounds();
		rect.y = y - rect.height;
		setSize(0, 0, 0, rect.width - width);

		g = image.createGraphics();
		g.setFont(fontType);
		g.setColor(Color.BLACK);
		g.drawString("" + ascii, 0, y);
		
		int left = 0;
		int right = 0;
		int top = 0;
		int bottom = 0;
		
		if (ascii != ' ')
		{
			for (; left < image.getWidth(); left++) {
				if (!isColEmpty(left))
					break;
			}

			for (int i = image.getWidth() - 1; i > 0; i--) {
				if (!isColEmpty(i))
					break;
				right++;
			}

			for (; top < image.getHeight(); top++) {
				if (!isRowEmpty(top))
					break;
			}

			for (int i = image.getHeight() - 1; i > 0; i--) {
				if (!isRowEmpty(i))
					break;
				bottom++;
			}
		}

		rect = new Rectangle(0, top, image.getWidth(), bottom);
		setSize(0, 0, -left, -right);
		// System.out.println(""+ascii+" yO="+rect.y+" yU="+(rect.y+rect.height)+" right="+right);

		return rect;
	}

	/**
	 * Returns the Description of this Character.
	 * 
	 * The Description is useful if the picture of this character differs
	 * from the normal character expected at this position (ASCII Code).
	 * 
	 * @return the Description of this Character
	 */
	public String getComment() {
		return comment;
	}

	/**
	 * Sets the Description of this Character.
	 * 
	 * @param comment		the Description of this Character
	 * @see   getComment()
	 */
	public void setComment(String comment) {
		this.comment = comment;
	}
	
	public BufferedImage getImage() {
		return image;
	}
	
	public void setImage(BufferedImage image) {
		this.image = image;
	}
}
