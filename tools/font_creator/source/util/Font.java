
package util;

import java.awt.Rectangle;
import java.awt.image.IndexColorModel;
import java.util.Vector;

/**
 * @author Fabian Maximilian Thiele
 */
public class Font {
	private int bits;
	private int height;
	private int width;
	private final int startIndex;
	private int hspace;
	private int vspace;
	
	private Vector<FontCharacter> chars;
	private String name;
	
	private IndexColorModel editorColorModel;
	
	public Font(int width, int initWidth, int height, int startIdx,
			int charCount, String name, int bits, java.awt.Font importFont) {
		chars = new Vector<FontCharacter>(charCount);
		this.width = width;
		this.height = height;
		this.name = name;
		this.startIndex = startIdx;
		this.hspace = 1;
		this.vspace = 0;
		this.bits = bits;

		editorColorModel = IndexColorModelUtils.getColorModel(bits);

		if (importFont != null) {
			this.height = height = importFont.getSize() * 2;
		}
		
		int cropTop = height;
		int cropBottom = height;
		for (int i = 0; i < charCount; i++) {
			FontCharacter c = new FontCharacter(initWidth, height, startIndex + i, this);
			
			if (importFont != null) {
				Rectangle rect = c.importChar(importFont);
				if ((char) c.getAsciiIndex() != ' ') {
					if (cropTop > rect.y)
						cropTop = rect.y;
					if (cropBottom > rect.height)
						cropBottom = rect.height;
				}
			}
			chars.addElement(c);
		}
		
		if (importFont != null) {
			for (int i = 0; i < charCount; i++) {
				chars.get(i).changeSize(-cropTop, -cropBottom, 0, 0);
			}
			this.height = height - cropTop - cropBottom; 
		}
		else {
			cropTop = 0;
			cropBottom = 0;
		}
	}
	
	public Font(SerializableFont sFont) {
		width = sFont.getWidth();
		height = sFont.getHeight();
		name = sFont.getName();
		startIndex = sFont.getStartIndex();
		hspace = sFont.getHorizontalSpace();
		vspace = sFont.getVerticalSpace();
		bits = sFont.getBits();
		
		editorColorModel = IndexColorModelUtils.getColorModel(bits);
		
		SerializableFontCharacter[] c = sFont.getChars();
		chars = new Vector<FontCharacter>(c.length);
		for (int i = 0; i < c.length; i++) {
			chars.add(c[i].getFontCharacter(startIndex + i, this));
		}
	}
	
	public FontCharacter getCharacter(int index) {
		return chars.get(index);
	}
	
	public FontCharacter[] getAllCharacters() {
		FontCharacter[] c = new FontCharacter[chars.size()];
		for (int i = 0; i < chars.size(); i++) {
			c[i] = chars.get(i);
		}
		return c;
	}

	public void setChar(FontCharacter fontChar, int index) {
		chars.setElementAt(fontChar, index);
	}

	public int getHeight() {
		return height;
	}

	public int getWidth() {
		return width;
	}

	public String getName() {
		return name;
	}

	public void setName(String string) {
		name = string;
	}

	public String toString() {
		return name;
	}

	public int getStartIndex() {
		return startIndex;
	}
	
	public int getCharCount() {
		return chars.size();
	}
	
	/**
	 * Horizontal spacing between two characters
	 */
	public int getHorizontalSpace() {
		return hspace;
	}
	
	/**
	 * Vertical spacing between two characters
	 */
	public int getVerticalSpace() {
		return vspace;
	}

	public void setHorizontalSpace(int i) {
		hspace = i;
	}
	
	public void setVerticalSpace(int i) {
		vspace = i;
	}
	
	public IndexColorModel getEditorColorModel() {
		return editorColorModel;
	}

	public int getBits() {
		return bits;
	}

	public int getSize() {
		int size = 0;
		for (int i = 0; i < chars.size(); i++)
		{
			FontCharacter fc = chars.get(i);
			int h = fc.getHeight() / 8;
			if (fc.getHeight() % 8 != 0)
				h = h + 1;
			size += fc.getWidth() * h * bits;
		}

		return size;
	}
}
