/*
 * Created on 29.03.2004
 *
 * To change the template for this generated file go to
 * Window&gt;Preferences&gt;Java&gt;Code Generation&gt;Code and Comments
 */
package util;

import java.awt.Graphics2D;
import java.awt.image.BufferedImage;
import java.awt.image.Raster;
import java.io.Serializable;

/**
 * @author Fabian Maximilian Thiele
 */
public class SerializableFontCharacter implements Serializable {
	private static final long serialVersionUID = -8336940559779433401L;
	private int width;
	private int height;
	private String comment;
	private int[] imageData;

	public SerializableFontCharacter(FontCharacter fc) {
		width = fc.getWidth();
		height = fc.getHeight();
		comment = fc.getComment();

		BufferedImage image = fc.getImage();
		if (image != null) {
			Raster data = image.getRaster();
			int[] temp = null;
			imageData = data.getPixels(0, 0, image.getWidth(),
					image.getHeight(), temp);
		}
	}
	
	public FontCharacter getFontCharacter(Font parentFont) {
		FontCharacter fc = new FontCharacter(width, height, parentFont);
		fc.setComment(comment);

		if (imageData != null) {
			BufferedImage image = new BufferedImage(width, height,
					BufferedImage.TYPE_BYTE_INDEXED,
					parentFont.getEditorColorModel());
			Graphics2D g = image.createGraphics();
			int x = 0;
			int y = 0;
			for (int i = 0; i < imageData.length; i++) {
				g.setColor(IndexColorModelUtils.getColorForIndex(
						parentFont.getEditorColorModel(), imageData[i]));
				g.drawLine(x, y, x, y);
				if (++x == width) {
					x = 0;
					y++;
				}
			}
			fc.setImage(image);
		}

		return fc;
	}
}
