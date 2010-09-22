/*
 * Created on 29.03.2004
 *
 * To change the template for this generated file go to
 * Window&gt;Preferences&gt;Java&gt;Code Generation&gt;Code and Comments
 */
package util;

import java.io.Serializable;

/**
 * @author Fabian Maximilian Thiele
 */
public class SerializableFont implements Serializable {
	private static final long serialVersionUID = -1609847421843752630L;
	private int bits;
	private int height;
	private int width;
	private int startIndex;
	private int charSpace;
	private int cropTop;
	private int cropBottom;
	private String name;
	private SerializableFontCharacter[] chars;

	public SerializableFont(Font font) {
		bits = font.getBits();
		height = font.getHeight();
		width = font.getWidth();
		startIndex = font.getStartIndex();
		charSpace = font.getCharSpace();
		name = font.getName();
		cropTop = font.getCropTop();
		cropBottom = font.getCropBottom();

		FontCharacter[] c = font.getAllChars();
		chars = new SerializableFontCharacter[c.length];
		for (int i = 0; i < chars.length; i++) {
			chars[i] = new SerializableFontCharacter(c[i]);
		}
	}

	public int getBits() {
		return bits;
	}

	public SerializableFontCharacter[] getChars() {
		return chars;
	}

	public int getCharSpace() {
		return charSpace;
	}

	public int getHeight() {
		return height;
	}

	public String getName() {
		return name;
	}

	public int getStartIndex() {
		return startIndex;
	}

	public int getWidth() {
		return width;
	}

	public int getCropBottom() {
		return cropBottom;
	}

	public int getCropTop() {
		return cropTop;
	}
}
