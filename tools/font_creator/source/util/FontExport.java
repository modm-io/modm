/*
 * Created on 30.03.2004
 *
 * To change the template for this generated file go to
 * Window&gt;Preferences&gt;Java&gt;Code Generation&gt;Code and Comments
 */
package util;

import gui.MainWindow;

import java.awt.image.BufferedImage;
import java.awt.image.Raster;
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.StringReader;
import java.util.Calendar;
import java.util.GregorianCalendar;

import javax.swing.JFileChooser;

/**
 * @author Fabian Maximilian Thiele
 */
public class FontExport {
	MainWindow mainWnd;

	public FontExport(MainWindow mainWnd) {
		this.mainWnd = mainWnd;
	}

	public void exportFont(Font font) {
		JFileChooser chooser = new JFileChooser(".");
		int rv = chooser.showSaveDialog(mainWnd);
		if (rv == JFileChooser.APPROVE_OPTION) {
			File dest = chooser.getSelectedFile();
			exportToCHeader(font, dest);
		}
	}

	private String formatNumber(String number) {
		if (number.length() == 1)
			number = "0" + number;
		return number;
	}

	private String hexStr(int number, int byteCount) {
		number >>= byteCount * 8;
		number &= 0xFF;

		String hexString = Integer.toHexString(number).toUpperCase();
		hexString = "0x" + formatNumber(hexString);

		return hexString;
	}

	private int getHeaderSize(String template, Font font) {
		int headerSize = 0;

		int idx = template.indexOf("\n?headerSize=");
		if (idx != -1) {
			int idx1 = template.indexOf(";", idx);
			headerSize += Integer.parseInt(template.substring(idx + 13, idx1));
		}

		idx = template.indexOf("\n?widthTable=");
		if (idx != -1) {
			int idx1 = template.indexOf(";", idx);
			boolean b = Boolean.parseBoolean(template.substring(idx + 13, idx1));
			if (b)
				headerSize += font.getCharCount();
		}

		return headerSize;
	}

	public void exportToCHeader(Font font, File destination) {
		try {
			BufferedReader in = new BufferedReader(new InputStreamReader(
					new FileInputStream("font.template")));
			String template = "";
			String line = in.readLine();
			while (line != null) {
				template += line + "\n";
				line = in.readLine();
			}
			in.close();

			String fontName = font.getName();
			String fileName = destination.getName();
			String defName = fontName.replaceAll(" ", "_").toUpperCase();
			GregorianCalendar cal = new GregorianCalendar();
			String date = formatNumber("" + cal.get(Calendar.DAY_OF_MONTH))
					+ "." + formatNumber("" + (cal.get(Calendar.MONTH) + 1))
					+ "." + +cal.get(Calendar.YEAR);
			String size = "" + (getHeaderSize(template, font) + font.getSize());
			String width = "" + font.getWidth();
			String height = ""
					+ (font.getHeight() - font.getCropTop() - font.getCropBottom());
			String bits = "" + font.getBits();
			String firstChar = "" + font.getStartIndex();
			String lastChar = "" + (font.getStartIndex() + font.getCharCount());
			String charCount = "" + font.getCharCount();

			template = template.replaceAll("%fontName", fontName);
			template = template.replaceAll("%fileName", fileName);
			template = template.replaceAll("%date", date);
			template = template.replaceAll("%size", size);
			template = template.replaceAll("%width", width);
			template = template.replaceAll("%height", height);
			template = template.replaceAll("%bits", bits);
			template = template.replaceAll("%firstChar", firstChar);
			template = template.replaceAll("%lastChar", lastChar);
			template = template.replaceAll("%charCount", charCount);
			template = template.replaceAll("%defName", defName);
			template = template.replaceAll("%name", fontName.replaceAll(" ",
					"_"));

			template = template.replaceAll("%binSize", hexStr(getHeaderSize(
					template, font)
					+ font.getSize(), 1)
					+ ", "
					+ hexStr(getHeaderSize(template, font) + font.getSize(), 0));
			template = template.replaceAll("%binWidth", hexStr(font.getWidth(),
					0));
			template = template.replaceAll("%binHeight",
					hexStr(font.getHeight() - font.getCropTop()
							- font.getCropBottom(), 0));
			template = template.replaceAll("%binFirstChar", hexStr(
					font.getStartIndex(), 0));
			template = template.replaceAll("%binCharCount", hexStr(
					font.getCharCount(), 0));

			String widthData = "";
			String fontData = "";
			FontCharacter[] fc = font.getAllCharacters();
			int newline = 1;

			for (int i = 0; i < fc.length; i++) {
				int w = fc[i].getWidth();
				widthData += hexStr(w, 0) + ", ";

				if (newline % 10 == 0) {
					widthData += System.getProperty("line.separator") + "    ";
					newline = 0;
				}
				newline++;

				BufferedImage img = fc[i].getImage();
				if (img != null) {
					Raster r = img.getData();
					int[] temp = null;
					int[] pixels = r.getPixels(0, 0, fc[i].getWidth(),
							fc[i].getHeight(), temp);

					int data, h = fc[i].getHeight();
					for (int j = 0; j < (h + 7) / 8; j++) {
						for (int k = 0; k < w; k++) {
							data = 0;
							// for(int l=0; l<8 && l+j*8<h; l++) {
							// int p = pixels[w*(j*8)+k+l*w];
							// data >>= 1;
							// data |= p<<7;
							// }
							for (int m = 0; m < 8 && m + j * 8 < h; m++) {
								int p = pixels[w * (j * 8) + k + m * w];
								data |= p << m;
							}
							fontData += hexStr(data, 0) + ", ";
							data = 0;
						}
					}

					if (i == fc.length - 1)
						fontData = fontData.substring(0, fontData.length() - 2)
								+ " ";
					fontData += "// " + (font.getStartIndex() + i);

					if (fc[i].getComment().length() > 0)
						fontData += "  " + fc[i].getComment();
					fontData += System.getProperty("line.separator") + "    ";
				}
			}
			template = template.replaceAll("%fontWidthData", widthData);
			template = template.replaceAll("%fontData", fontData);

			BufferedWriter out = new BufferedWriter(new OutputStreamWriter(
					new FileOutputStream(destination)));
			in = new BufferedReader(new StringReader(template));
			line = in.readLine();
			while (line != null) {
				if (!line.startsWith("?")) {
					out.write(line);
					out.newLine();
				}
				line = in.readLine();
			}
			out.close();
		} catch (IOException e) {
			System.err.println("IOException: " + e.toString());
		}
	}
}
