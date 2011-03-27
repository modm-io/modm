
package util;

import java.awt.Graphics2D;
import java.awt.image.BufferedImage;
import java.awt.image.Raster;
import java.io.IOException;
import java.io.BufferedReader;
import java.io.Serializable;
import java.io.Writer;
import java.util.regex.Pattern;
import java.util.regex.Matcher;

/**
 * 
 * @author Fabian Maximilian Thiele
 * @author Fabian Greif
 */
public class SerializableFontCharacter implements Serializable {
	private static final long serialVersionUID = -8336940559779433401L;
	private int width;
	private int height;
	private int asciiIndex;
	private String comment;
	private int[] imageData;

	public SerializableFontCharacter(FontCharacter fc) {
		width = fc.getWidth();
		height = fc.getHeight();
		asciiIndex = fc.getAsciiIndex();
		comment = fc.getComment();
		
		BufferedImage image = fc.getImage();
		if (image != null) {
			Raster data = image.getRaster();
			int[] temp = null;
			imageData = data.getPixels(0, 0, image.getWidth(),
					image.getHeight(), temp);
		}
	}
	
	public SerializableFontCharacter(BufferedReader reader, int index, int height) throws IOException {
		this.asciiIndex = index;
		this.height = height;
		width = -1;
		
		Pattern p = Pattern.compile("^\\[([ #]*)\\].*");
		for (int i = 0; i < height; i++) {
			String line = reader.readLine();
			Matcher m = p.matcher(line);
			
			if (m.matches()) {
				String pixels = m.group(1);
				int length = pixels.length();
				
				if (width == -1) {
					width = length;
					imageData = new int[width * height];
				}
				else if (width != length) {
					throw new IOException("Width differs inside the character " + index);
				}
				
				int k = 0;
				for (char c: pixels.toCharArray()) {
					int value = 0;
					if (c == '#') {
						value = 1;
					}
					imageData[width * i + k] = value;
					k += 1;
				}
			}
			else {
				throw new IOException("Unknown format for character " + index);
			}
		}
	}
	
	public FontCharacter getFontCharacter(int index, Font parentFont) {
		FontCharacter fc = new FontCharacter(width, height, index, parentFont);
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
	
	public void writeToFile(Writer writer) throws IOException {
		if (asciiIndex >= 32 && asciiIndex < 127) {
			writer.write("\n#char : " + asciiIndex + " '" + (char) asciiIndex + "'\n");
		}
		else {
			writer.write("\n#char : " + asciiIndex + "\n");
		}
		
		int i = 0;
		for (int pixel: imageData) {
			if (i == 0) {
				writer.write("[");
			}
			
			if (pixel == 0) {
				writer.write(" ");
			}
			else {
				writer.write("#");
			}
			
			i++;
			if (i >= width) {
				i = 0;
				writer.write("]\n");
			}
		}
	}
}
