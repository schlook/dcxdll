/*!
 * \file dcxml.cpp
 * \brief blah
 *
 * TODO: rename all variables to use the same convention (camelCase)
 *
 * \author Martijn Laarman ( Mpdreamz at mirc dot net )
 * \version 2.2
 *
 * \b Revisions
 *        -Completely rewrote DCXML to be more OOP
 *		  -
 *
 * � ScriptsDB.org - 2006
 */
#include "dcxmlincludes.h"
#include "dcxmlparser.h"

/*
dcxml [-FLAGS] [NAME] [DNAME] "[PATH]"
*/
DcxmlParser::DcxmlParser() {

};
DcxmlParser::DcxmlParser(const char *file,const char *mark,const char *dialogName, bool verbose, bool autoClose)
{
	this->loadSuccess = true;
	this->isVerbose(verbose);
	this->isAutoClose(autoClose);

	this->setFilePath(file);

	this->setDialogMark(mark);
	this->setDialogName(dialogName);

	if (!this->loadDialog()
		|| !this->loadDocument()
		|| !this->loadDialogElement()
		)
	{
		this->loadSuccess = false;
		if ((this->getDialog() != NULL && this->isAutoClose()))
			Dcx::mIRC.execex("/xdialog -x %s",this->getDialogName());

		return;
	}

	Dcx::mIRC.execex("/dialog -s %s %i %i %i %i", //!< Sets the dialog size.
		this->getDialogMark(),
		this->queryIntAttribute(this->getDialogElement(),"x",-1),
		this->queryIntAttribute(this->getDialogElement(),"y",-1),
		this->queryIntAttribute(this->getDialogElement(),"w",-1),
		this->queryIntAttribute(this->getDialogElement(),"h",-1)
	);

	this->setZlayered(false);

	const char *cascade = this->queryAttribute(this->getDialogElement(),"cascade","v");
	const char *margin = this->queryAttribute(this->getDialogElement(),"margin","0 0 0 0");
	const char *caption = this->queryAttribute(this->getDialogElement(),"caption",this->getDialogName());
	const char *border = this->getDialogElement()->Attribute("border");
	this->controls = 0;
	//margin = (temp = element->Attribute("padding")) ? temp : "0 0 0 0";

	if (this->getDialogElement()->Attribute("border")) //!< set border ONLY if defined on <dialog>
		this->xdialogEX("-b","+%s",border);

	Dcx::mIRC.execex("//dialog -t %s %s", this->getDialogMark(), caption); 
	this->xdialogEX("-l","root \t +p%s 0 0 0 0", cascade);
	this->xdialogEX("-l","space root \t + %s", margin);
	
	this->parseDialog(); //!< Parse <dialog> children onto the dialog

	Dcx::mIRC.execex("/.timer 1 0 xdialog -l %s update", this->getDialogMark()); //!< Update CLA After DCXML is drawn.
	
	if (this->getDialogElement()->Attribute("center")) //!< Centers the dialog
		Dcx::mIRC.execex("/dialog -r %s", this->getDialogMark());

	//This "Shite" is to activate the first zlayer, added a check if this command starts returning an error
	if (this->getZlayered()) this->xdialogEX("-z","+s 1");
};

void DcxmlParser::setDialog(const char *dialogMark) { this->_dcxDialog = Dcx::Dialogs.getDialogByName(dialogMark);	};

bool DcxmlParser::loadDocument()
{
	if (!Dcx::isFile(this->getFilePath()))
	{
		Dcx::errorex("/dcxml", "File \"%s\" does not exist or is in use by another process ", this->getFilePath());
		return false;
	}
	TiXmlBase::SetCondenseWhiteSpace(false); 
	TiXmlDocument doc(this->getFilePath());
	this->loadSuccess = doc.LoadFile();
	if (!this->loadSuccess) { 
		Dcx::errorex("/dcxml", "XML error in \"%s\" (row %i column %i) %s",
			this->getFilePath(),
			this->getDocument()->ErrorRow(),
			this->getDocument()->ErrorCol(),
			this->getDocument()->ErrorDesc());
		return false;
	}
	this->_document = doc; 
	return true;
}

bool DcxmlParser::loadDialog()
{
	this->setDialog(this->getDialogMark());
	if (this->getDialog() == NULL)
	{
		Dcx::errorex("/dcxml", "No such dialog (\"%s\") has been marked with dcx.",this->getDialogMark());
		return false;
	}
	return true;
}
bool DcxmlParser::loadDialogElement()
{
	this->setRootElement(this->getDocument()->FirstChildElement("dcxml"));
	if (!this->getRootElement())
	{
		Dcx::errorex("/dcxml", "The document element should be <dcxml> in \"%s\"", this->getFilePath());
		return false;
	}
	this->setDialogsElement(this->getRootElement()->FirstChildElement(std::string("dialogs")));
	if (!this->getDialogsElement()) 
	{ 
		Dcx::errorex("/dcxml", "Theres no <dialogs> child for <dcxml> in \"%s\"", this->getFilePath());
		return false;
	}
	/*
	 * This finds the Dialog element. 
	 * Important Note:
	 * The for loop has an ugly side assigment: it sets this->element 
	 * which is used as a recursion cursor for this->parseDialog()
	 */
	bool found = false;
	for(	this->element = this->getDialogsElement()->FirstChildElement("dialog");
			this->element;
			this->element = this->element->NextSiblingElement("dialog") 
		)	if (0==lstrcmp(this->element->Attribute("name"), this->getDialogName()))
	{ 
		found = true;
		this->setDialogElement(this->element);
		break;
	}
	if (!found) 
	{
		Dcx::errorex("/dcxml", "Theres no <dialog> element with attribute name=\"%s\" in \"%s\"", this->getDialogName(), this->getFilePath());
		return false;
	}
	return true;
};


const char *DcxmlParser::queryAttribute(TiXmlElement *element,const char *attribute,const char *defaultValue)
{
	const char *t;
	return (t = element->Attribute(attribute)) ? t : defaultValue;
};
int DcxmlParser::queryIntAttribute(TiXmlElement *element,const char *attribute,int defaultValue)
{
	int integer;
	return (element->QueryIntAttribute(attribute,&integer) == TIXML_SUCCESS) ? integer : defaultValue;
};

void DcxmlParser::parseAttributes() {
    elem = element->Value();
    parentelem = parent->Value();
    parenttype = (temp = parent->Attribute("type")) ? temp : "panel";
    type = (temp = element->Attribute("type")) ? temp : "panel";
    STclass = (temp = element->Attribute("class")) ? temp : "";
    weigth = (temp = element->Attribute("weight")) ? temp : "1";
    height = (temp = element->Attribute("height")) ? temp : "0";
    width = (temp = element->Attribute("width")) ? temp : "0";
    margin = (temp = element->Attribute("margin")) ? temp : "0 0 0 0";
    styles = (temp = element->Attribute("styles")) ? temp : "";
	caption = (temp = element->Attribute("caption")) ? temp : (temp = element->GetText()) ? temp : "";
	tooltip = (temp = element->Attribute("tooltip")) ? temp : "";
    cascade = (temp = element->Attribute("cascade")) ? temp : "";
    icon = (temp = element->Attribute("icon")) ? temp : "0";
    integral = (temp = element->Attribute("integral")) ? temp : "0";
    state = (temp = element->Attribute("state")) ? temp : "0";
    indent = (temp = element->Attribute("indent")) ? temp : "0";
    //flags attribute defaults different per type/item
    tFlags = element->Attribute("flags");
    src = (temp = element->Attribute("src")) ? temp : "";
    cells = (temp = element->Attribute("cells")) ? temp : "-1";
    rebarMinHeight = (temp = element->Attribute("minheight")) ? temp : "0";
    rebarMinWidth = (temp = element->Attribute("minwidth")) ? temp : "0";
    iconsize = (temp = element->Attribute("iconsize")) ? temp : "16";
    eval = (element->QueryIntAttribute("eval",&eval) == TIXML_SUCCESS) ? eval : 0;

    fontstyle = (temp = element->Attribute("fontstyle")) ? temp : "d";
    charset = (temp = element->Attribute("charset")) ? temp : "ansi";
    fontsize = (temp = element->Attribute("fontsize")) ? temp : "";
    fontname = (temp = element->Attribute("fontname")) ? temp : "";
    border = (temp = element->Attribute("border")) ? temp : "";
    cursor = (temp = element->Attribute("cursor")) ? temp : "arrow";
    bgcolour = (temp = element->Attribute("bgcolour")) ? temp : "0";
    textbgcolour = (temp = element->Attribute("textbgcolour")) ? temp : "";
    textcolour = (temp = element->Attribute("textcolour")) ? temp : "0";

	gradientstart = (temp = element->Attribute("gradientstart")) ? temp : "";
	gradientend = (temp = element->Attribute("gradientend")) ? temp : "";

    disabledsrc = (temp = element->Attribute("disabledsrc")) ? temp : "";
    hoversrc = (temp = element->Attribute("hoversrc")) ? temp : "";
    selectedsrc = (temp = element->Attribute("selectedsrc")) ? temp : "";
}
void DcxmlParser::parseAttributes(TiXmlElement* element) {
    elem = element->Value();
    parentelem = parent->Value();
    parenttype = (temp = parent->Attribute("type")) ? temp : "panel";
    weigth = (temp = element->Attribute("weight")) ? temp : "1";
    height = (temp = element->Attribute("height")) ? temp : "0";
    width = (temp = element->Attribute("width")) ? temp : "0";
    margin = (temp = element->Attribute("margin")) ? temp : "0 0 0 0";
    styles = (temp = element->Attribute("styles")) ? temp : "";
    caption = (temp = element->Attribute("caption")) ? temp : (temp = element->GetText()) ? temp : "";
    tooltip = (temp = element->Attribute("tooltip")) ? temp : "";
    cascade = (temp = element->Attribute("cascade")) ? temp : "";
    icon = (temp = element->Attribute("icon")) ? temp : "0";
    integral = (temp = element->Attribute("integral")) ? temp : "0";
    state = (temp = element->Attribute("state")) ? temp : "0";
    indent = (temp = element->Attribute("indent")) ? temp : "0";
    //flags attribute defaults different per type/item
    tFlags = element->Attribute("flags");
    src = (temp = element->Attribute("src")) ? temp : "";
    cells = (temp = element->Attribute("cells")) ? temp : "-1";
    rebarMinHeight = (temp = element->Attribute("minheight")) ? temp : "0";
    rebarMinWidth = (temp = element->Attribute("minwidth")) ? temp : "0";
    iconsize = (temp = element->Attribute("iconsize")) ? temp : "16";
    eval = (element->QueryIntAttribute("eval",&eval) == TIXML_SUCCESS) ? eval : 0;

    fontstyle = (temp = element->Attribute("fontstyle")) ? temp : "d";
    charset = (temp = element->Attribute("charset")) ? temp : "ansi";
    fontsize = (temp = element->Attribute("fontsize")) ? temp : "";
    fontname = (temp = element->Attribute("fontname")) ? temp : "";
    border = (temp = element->Attribute("border")) ? temp : "";
    cursor = (temp = element->Attribute("cursor")) ? temp : "arrow";
    bgcolour = (temp = element->Attribute("bgcolour")) ? temp : "0";
    textbgcolour = (temp = element->Attribute("textbgcolour")) ? temp : "";
    textcolour = (temp = element->Attribute("textcolour")) ? temp : "0";

	gradientstart = (temp = element->Attribute("gradientstart")) ? temp : "";
	gradientend = (temp = element->Attribute("gradientend")) ? temp : "";

    disabledsrc = (temp = element->Attribute("disabledsrc")) ? temp : "";
    hoversrc = (temp = element->Attribute("hoversrc")) ? temp : "";
    selectedsrc = (temp = element->Attribute("selectedsrc")) ? temp : "";
}
    /* parseControl() : if current element is a control perform some extra commands*/
void DcxmlParser::parseControl() { 
	//DcxControl *control = this->getDialog()->getControlByID(id + mIRC_ID_OFFSET);
    if (element->Attribute("zlayer")) { 
        this->xdialogEX("-z","+a %i",id);
        this->setZlayered(true);
    }
//        padding = (temp = element->Attribute("padding")) ? temp : "0 0 0 0";

	if ((0==lstrcmp(parenttype, "divider")) && (element->Attribute("width"))) {
        const char *width = (temp = element->Attribute("width")) ? temp : "100";
        xdidEX(parentid,"-v","%s",width);
    }
    if (((0==lstrcmp(type, "toolbar")) || (0==lstrcmp(type, "button")))
        || (0==lstrcmp(type, "treeview"))) { 
        xdidEX(id,"-l","%s",iconsize);
    }
    if (0==lstrcmp(type, "toolbar")) this->parseItems(element);
    else if (0==lstrcmp(type, "treeview")) this->parseItems(element);
    else if (0==lstrcmp(type, "comboex")) this->parseItems(element);
    else if (0==lstrcmp(type, "list")) this->parseItems(element);
    else if (0==lstrcmp(type, "listview")) this->parseItems(element);

    if (((((0==lstrcmp(type, "box")) || (0==lstrcmp(type, "check")))
        || (0==lstrcmp(type, "link"))) || (0==lstrcmp(type, "radio"))) || (0==lstrcmp(type, "button")))
         { 
             if (caption) xdidEX(id,"-t","%s",caption);
    }
    if ((0==lstrcmp(type, "ipaddress")) && (caption))
        this->xdidEX(id,"-a","%s",caption);
    if ((0==lstrcmp(type, "webctrl")) && (src))
        this->xdidEX(id,"-n","%s",src);
    else if (0==lstrcmp(type, "text")) { 
        if (caption) { 
            TString mystring(caption);
            if (mystring.left(2) == "\r\n") mystring = mystring.right(-2);
            else if (mystring.left(1) == "\n") mystring = mystring.right(-1);
            mystring.replace("\t","");
            TString printstring;
            int textspace = 0;
            while(mystring.gettok(1) != "") { 
                printstring.addtok(mystring.gettok(1).to_chr());
                if (printstring.len() > 800) { 
                    this->xdidEX(id,"-a","%i %s",textspace,printstring.gettok(1,-1).to_chr());
                    printstring = "";
                    textspace = 1;
                }
                mystring.deltok(1);
            }
            if (printstring != "") { 
                this->xdidEX(id,"-a","%i %s",textspace,printstring.gettok(1,-1).to_chr());
            }
        }
    }
    else if (0==lstrcmp(type, "edit")) { 
        if (caption) { 
            TString mystring(caption);
            if (mystring.left(2) == "\r\n") mystring = mystring.right(-2);
            else if (mystring.left(1) == "\n") mystring = mystring.right(-1);
            mystring.replace("\t","");
            int line = 0;
            while(mystring.gettok(1,"\r\n") != "") { 
                line++;
                this->xdidEX(id,"-i","%i %s",line,mystring.gettok(1,"\r\n").to_chr());
                mystring.deltok(1,"\r\n");
            }
        }
    }
    else if (0==lstrcmp(type, "richedit")) { 
        if (caption) { 
            TString mystring(caption);
            if (mystring.left(2) == "\r\n") mystring = mystring.right(-2);
            else if (mystring.left(1) == "\n") mystring = mystring.right(-1);
            mystring.replace("\t","");
            mystring.replace("\\c","");
            mystring.replace("\\b","");
            mystring.replace("\\r","");
            int line = 0;
            while(mystring.gettok(1,"\r\n") != "") { 
                line++;
                this->xdidEX(id,"-i","%i %s",line,mystring.gettok(1,"\r\n").to_chr());
                mystring.deltok(1,"\r\n");
            }
        }
    }
    else if (0==lstrcmp(type, "pbar")) { 
        if (caption) {
            this->xdidEX(id,"-i","%s",caption);
        }
    }
    else if (0==lstrcmp(type, "image")) { 
        const char *flags = (tFlags) ? tFlags : "";
        this->xdidEX(id,"-i","+%s %s",flags,src);
    }
    else if (0==lstrcmp(type, "statusbar")) { 
        this->xdidEX(id,"-l","%s",cells);
        this->parseItems(element);
    }
    disabledsrc = (temp = element->Attribute("disabledsrc")) ? temp : "";
    hoversrc = (temp = element->Attribute("hoversrc")) ? temp : "";
}
	/* xdialogEX(switch,format[,args[]]) : performs an xdialog command internally or trough mIRC */
void DcxmlParser::xdialogEX(const char *sw,const char *dFormat, ...) { 
        va_list args;
		TString txt;

		va_start(args, dFormat);
		txt.vprintf(dFormat, &args);
		va_end(args);

		if (this->isVerbose())
			Dcx::mIRC.execex("/echo -a dcxml debug: /xdialog %s %s %s",sw,this->getDialogMark(),txt);

		if (eval) Dcx::mIRC.execex("//xdialog %s %s %s",sw,this->getDialogMark(),txt);
        else this->getDialog()->parseCommandRequestEX("%s %s %s",this->getDialogMark(),sw,txt);
}
	/* xdidEX(controlId,switch,format[,args[]]) : performs an xdid command internally or trough mIRC on the specified id */
void DcxmlParser::xdidEX(int id,const char *sw,const char *dFormat, ...) { 
        va_list args;
		TString txt;

		va_start(args, dFormat);
		txt.vprintf(dFormat, &args);
		va_end(args);
		
		if (this->isVerbose())
			Dcx::mIRC.execex("/echo -a dcxml debug: /xdid %s %s %i %s",sw,this->getDialogMark(),id,txt);

        if (eval) Dcx::mIRC.execex("//xdid %s %s %i %s",sw,this->getDialogMark(),id,txt);
        else this->getDialog()->parseComControlRequestEX(id,"%s %i %s %s",this->getDialogMark(),id,sw,txt);
}
    /* parseCLA(int numberOfClaControlsInCurrentBranch) : parses control and pane elements and applies the right CLA commands */
TString DcxmlParser::parseCLA(const int cCla) { 
    if (0==lstrcmp(elem, "control")) { 
        if ((0==lstrcmp(type, "panel")) || (0==lstrcmp(type, "box"))) {
            this->xdidEX(id,"-l","root \t +p%s 0 0 0 0",cascade);
            this->xdidEX(id,"-l","space root \t + %s",margin);
            g_resetcla = 1;
        }
        const char * fHeigth = "";
        const char * fWidth = "";
        const char * fixed = "l";
        if (element->Attribute("height")) { fHeigth = "v"; fixed = "f"; weigth = "0"; }
        if (element->Attribute("width")) { fWidth = "h"; fixed = "f"; weigth = "0"; }
        if (0==lstrcmp(parentelem, "dialog"))
        {
            this->xdialogEX("-l","cell %s \t +%s%s%si %i %s %s %s",
                g_claPath,fixed,fHeigth,fWidth,id,weigth,width,height);
        }
        else if (0==lstrcmp(parentelem, "control")) {
            if ((parent->Attribute("type")) && (parentid)) {
                if (0==lstrcmp(parent->Attribute("type"), "panel")) { 
                    this->xdidEX(parentid,"-l","cell %s \t +%s%s%si %i %s %s %s",
                        g_claPath,fixed,fHeigth,fWidth,id,weigth,width,height); 
                }
                else if (0==lstrcmp(parent->Attribute("type"), "box")) { 
                    this->xdidEX(parentid,"-l","cell %s \t +%s%s%si %i %s %s %s",
                        g_claPath,fixed,fHeigth,fWidth,id,weigth,width,height); 
                }
            }
        }
    }
    else if (0==lstrcmp(elem, "pane")) {
        if (0==lstrcmp(parentelem, "dialog")) { 
            this->xdialogEX("-l","cell %s \t +p%s 0 %s 0 0",g_claPath,cascade,weigth);
        }
        if (0==lstrcmp(parentelem, "control")) {
            if ((parenttype) && (parentid)) {
                if (0==lstrcmp(parenttype, "panel"))
                    this->xdidEX(parentid,"-l","cell %s \t +p%s 0 %s 0 0",g_claPath,cascade,weigth);
                else if (0==lstrcmp(parenttype, "box"))
                    this->xdidEX(parentid,"-l","cell %s \t +p%s 0 %s 0 0",g_claPath,cascade,weigth);
            }
        }
    }
    char buffer [100];
    const char * claPathx = 0;
    if (g_resetcla) {
        lstrcpy(buffer, "root");
        claPathx = buffer;
    }
    else if (0==lstrcmp(g_claPath, "root")) {
        wsprintf (buffer, "%i",cCla);
        claPathx = buffer;
    }
    else { 
        wsprintf (buffer, "%s %i",g_claPath,cCla);
        claPathx = buffer;
    }
    if (element->Attribute("margin")) {
        if (0==lstrcmp(parentelem, "dialog"))
            this->xdialogEX("-l","space %s \t + %s",claPathx,margin);
        else this->xdidEX(parentid,"-l","space %s \t + %s",g_claPath,margin);
    }
    g_resetcla = 0;
    return TString(claPathx);
}
    /* setStyle(TiXmlElement*) : Applies the styles described on the element found by parseStyle() */
void DcxmlParser::setStyle(TiXmlElement* style) {
    //style attributes evaluate by default unless eval="0" is set on the element explicitly

	eval = (style->QueryIntAttribute("eval",&eval) == TIXML_SUCCESS) ? eval : 1;
	
	//font
    fontstyle = (temp = style->Attribute("fontstyle")) ? temp : "d";
    charset = (temp = style->Attribute("charset")) ? temp : "ansi";
    fontsize = (temp = style->Attribute("fontsize")) ? temp : "";
    fontname = (temp = style->Attribute("fontname")) ? temp : "";
    if ((style->Attribute("fontsize")) || (style->Attribute("fontname")))
        this->xdidEX(id,"-f","+%s %s %s %s",
            fontstyle,charset,fontsize,fontname);
    //border
    border = (temp = style->Attribute("border")) ? temp : "";
    if (style->Attribute("border")) this->xdidEX(id,"-x","+%s",border);
    //colours
    cursor = (temp = style->Attribute("cursor")) ? temp : "arrow";
    bgcolour = (temp = style->Attribute("bgcolour")) ? temp : "";
    textbgcolour = (temp = style->Attribute("textbgcolour")) ? temp : "";
    textcolour = (temp = style->Attribute("textcolour")) ? temp : "";
    if (style->Attribute("bgcolour")) { 
        this->xdidEX(id,"-C","+b %s",bgcolour);
        if (0==lstrcmp(type, "pbar")) 
		{
			this->xdidEX(id,"-k","%s",bgcolour);
			this->xdidEX(id,"-U","%s","");
		}
    }
    if (style->Attribute("textbgcolour")) 
	{
		this->xdidEX(id,"-C","+k %s",textbgcolour);
		if (0==lstrcmp(type, "pbar")) 
		{
			this->xdidEX(id,"-c","%s",textbgcolour);
			this->xdidEX(id,"-U","%s","");
		}
	}
    else if (style->Attribute("bgcolour")) this->xdidEX(id,"-C","+k %s",bgcolour); 
    if (style->Attribute("textcolour")) 
	{
		xdidEX(id,"-C","+t %s",textcolour);
		if (0==lstrcmp(type, "pbar")) 
		{
			this->xdidEX(id,"-q","%s",textcolour);
			this->xdidEX(id,"-U","%s","");
		}
	}
	
	if (style->Attribute("gradientstart")) this->xdidEX(id,"-C","+g %s",gradientstart);
	if (style->Attribute("gradientend")) this->xdidEX(id,"-C","+G %s",gradientend);

    //cursor
    if (style->Attribute("cursor")) this->xdidEX(id,"-J","+r %s",cursor);    

    //iconsize
    if (style->Attribute("iconsize")) 
    { 
        if (((0==lstrcmp(type, "toolbar")) || (0==lstrcmp(type, "button")))
            || (0==lstrcmp(type, "treeview"))) 
        { 
            this->xdidEX(id,"-l","%s",iconsize);
        }
    }
    if (0==lstrcmp(type, "button"))
    {
        if (!element->Attribute("bgcolour")) bgcolour = "65280";
        if (element->Attribute("src"))
        {
            this->xdidEX(id,"-k","+n %s %s",bgcolour,src);
        }
        if (element->Attribute("disabledsrc"))
        {
            this->xdidEX(id,"-k","+n %s %s",bgcolour,disabledsrc);
        }
        if (element->Attribute("hoversrc"))
        {
            this->xdidEX(id,"-k","+n %s %s",bgcolour,hoversrc);
        }
        if (element->Attribute("selectedsrc"))
        {
            this->xdidEX(id,"-k","+n %s %s",bgcolour,hoversrc);
        }
    }
}

	/* parseStyle(recursionDepth) : Simple recursive method to cascade find the right style to apply to an element */
void DcxmlParser::parseStyle(int depth) { 
    if (depth > 2) return;
    depth++;
    TiXmlElement* styles = 0;
    TiXmlElement* style = 0;
    TiXmlElement* ClassElement = 0;
    TiXmlElement* TypeElement = 0;
    TiXmlElement* IdElement = 0;
    if (depth == 3) style = this->element;
    if (depth == 1) styles = this->getDialogsElement()->FirstChildElement("styles");
    if (depth == 2) styles = this->getDialogElement()->FirstChildElement("styles");
    if (styles) {
        if (style = styles->FirstChildElement("all")) setStyle(style);
        style = 0;
        for( style = styles->FirstChildElement("style"); style; style = style->NextSiblingElement()) {
            if (0==lstrcmp(style->Attribute("class"), STclass)) ClassElement = style;
            if (0==lstrcmp(style->Attribute("type"), type)) TypeElement = style;
            int t_id = parseId(style);
            if (t_id == id) IdElement = style;
        }
        if (IdElement) style = IdElement;
        else if (ClassElement) style = ClassElement;
        else if (TypeElement) style = TypeElement;
    }
    if (style) this->setStyle(style);
    this->parseStyle(depth);
}
	/* parseIcons(recursionDepth) : Simple recursive method to cascade find the right icons to apply to an element */
void DcxmlParser::parseIcons(int depth) { 
    if (depth > 1) return;
    depth++;
    TiXmlElement* icons = 0;
    TiXmlElement* icon = 0;
    TiXmlElement* ClassElement = 0;
    TiXmlElement* TypeElement = 0;
    TiXmlElement* IdElement = 0;
    if (depth == 1) icons = this->getDialogElement()->FirstChildElement("icons");
    if (depth == 2) icons = this->getDialogsElement()->FirstChildElement("icons");
    if (icons) {
        for( icon = icons->FirstChildElement("icon"); icon; icon = icon->NextSiblingElement()) {
            if (0==lstrcmp(icon->Attribute("class"), STclass)) ClassElement = icon;
            if (0==lstrcmp(icon->Attribute("type"), type)) TypeElement = icon;
            int t_id = this->parseId(icon);
            if (t_id == id) IdElement = icon;
        }
        if (IdElement) icon = IdElement;
        else if (ClassElement) icon = ClassElement;
        else if (TypeElement) icon = TypeElement;
        else icon = 0;
        if (icon) { 
            const char *flags = (temp = icon->Attribute("flags")) ? temp : "ndhs";
            const char *index = (temp = icon->Attribute("index")) ? temp : "0";
            const char *src = icon->Attribute("src");
            int indexmin = (icon->QueryIntAttribute("indexmin",&indexmin) == TIXML_SUCCESS) ? indexmin : 0;
            int indexmax = (icon->QueryIntAttribute("indexmax",&indexmax) == TIXML_SUCCESS) ? indexmax : -1;
            if (src) { 
                if (indexmin <= indexmax) 
                    //method sucks but looping in C++ is WAYYY too fast for mIRC
				{
                        Dcx::mIRC.execex("//var %%x = %i | while (%%x <= %i ) { xdid -w %s %i +%s %%x %s | inc %%x }",
						indexmin,indexmax,this->getDialogMark(),id,flags,src);
				}
                else 
					//we need to use execex to evaluate identifiers in the path
					Dcx::mIRC.execex("//xdid -w %s %i +%s %s %s",this->getDialogMark(),id,flags,index,src);
					//this->xdidEX(id,"-w","+%s %s %s",flags,index,src);
            }
        }
    }
    else parseIcons(depth);
}
	/* parseItems(XmlElement,recursionDepth,itemPath) : recursively applies items for a control */
void DcxmlParser::parseItems(TiXmlElement* element,int depth,char *itemPath) { 
    int item = 0;
    int cell = 0;
    TiXmlElement* child = 0;
	
	for( child = element->FirstChildElement(); child; child = child->NextSiblingElement() ) {
        const char *childelem = child->Value();
		if (0==lstrcmp(childelem, "columns"))
		{
			if (0==lstrcmp(type, "listview"))
			{
				TString arguments;
				for(TiXmlElement *column = child->FirstChildElement("column");
					column; 
					column = column->NextSiblingElement("column") ) 
				{

					const char *width = (temp = column->Attribute("width")) ? temp : "0";
					const char *caption = (temp = column->Attribute("caption")) ? temp : "";
					const char *flags = (temp = column->Attribute("flags")) ? temp : "l";
					const char *icon = (temp = column->Attribute("icon")) ? temp : "0";
					
					const char *args;
					char buffer [512]; //!< if anyone needs a bigger column name tough luck!
					wsprintf (buffer, "+%s %s %s %s ",flags,icon,width,caption);
					args = buffer;
					arguments.addtok(args,"\t");
				}
				if (arguments.numtok() > 0)
					this->xdidEX(id,"-t","%s",arguments.to_chr());
			}
			if (0==lstrcmp(childelem, "dataset"))
			{
				//DcxListView *listView = this->d_Host->getControlByID(id);
			}
		}


        if ((0==lstrcmp(childelem, "item")) || (0==lstrcmp(childelem, "control")))
            cell++;
        //fill all required parameters with attributes or default values
        this->parseAttributes(child);
        if (0==lstrcmp(childelem, "item")) { 
            item++;            
            if (0==lstrcmp(type, "toolbar")) { 
                const char *flags = (tFlags) ? tFlags : "a";
                this->xdidEX(id,"-a","0 +%s %s %s %s %s \t %s",
                    flags,width,icon,textcolour,caption,tooltip);
            }
            else if (0==lstrcmp(type, "comboex")) { 
               this-> xdidEX(id,"-a","0 %s %s %s 0 %s",
                    indent,icon,icon,caption);
            }
            else if (0==lstrcmp(type, "list")) { 
                this->xdidEX(id,"-a","0 %s",
                    caption);
            }
            else if (0==lstrcmp(type, "statusbar")) { 
                const char *flags = (tFlags) ? tFlags : "f";
                this->xdidEX(id,"-t","%i +%s %s %s \t %s",
                    cell,flags,icon,caption,tooltip);
            }
            else if (0==lstrcmp(type, "treeview")) { 
                const char *flags = (tFlags) ? tFlags : "a";
                char buffer [100];
                char * pathx = 0;
                wsprintf (buffer, "%s %i",itemPath,item);
                pathx = buffer;
                this->xdidEX(id,"-a","%s \t +%s %s %s 0 %s %s %s %s %s \t %s",
                    pathx,flags,icon,icon,state,integral,textcolour,bgcolour,caption,tooltip);
                this->parseItems(child,depth,pathx);
            }
        }
    }
}

	/* parseTemplate(recursionDepth,claPath,firstFreeControlId) : finds a template and parses it into the current dialog */
 void DcxmlParser::parseTemplate(int dialogDepth,const char *claPath,int passedid)
{
    TiXmlElement* Template = 0;
    TiXmlElement* lookIn = 0;
    TiXmlElement* found = 0;
    lookIn = this->getDialogsElement()->FirstChildElement("templates");
    for (Template = lookIn->FirstChildElement("template");Template;Template = Template->NextSiblingElement()) 
    {
        if (0==lstrcmp(Template->Attribute("name"), element->Attribute("name")))
        { 
            found = Template;
            break;
        }
    }
    if (found) 
    {
        element = found;
        this->parseDialog(dialogDepth,claPath,passedid,1);
    }
}
	/* parseDialog(recursionDepth,claPath,firstFreeControlId,ignoreParentFlag) : finds a template and parses it into the current dialog */
void DcxmlParser::parseDialog(int depth,const char *claPath,int passedid,int ignoreParent) { 
    TiXmlElement* child = 0;
    int control = 0;
    g_claPath = 0;
    g_claPathx = 0;
    g_resetcla = 0;
    int cCla = 0;
    int cell = 0;
    for( child = this->element->FirstChildElement(); child; child = child->NextSiblingElement() ) {
        cell++;
        //STEP 1: SET ELEMENT AND PARENTELEMENT
        if (!ignoreParent) this->parent = child->Parent()->ToElement();
        this->element = child->ToElement();

        //STEP 2: PARSE ATTRIBUTES OF ELEMENTS
        this->parseAttributes();

        //dont itterate over unneccessary items
        if (0==lstrcmp(elem, "calltemplate")) 
        {
            if (0!=lstrcmp("template",element->Parent()->ToElement()->Value()))
            {
                cCla++;
                templateRef = element;
                templateRefcCla = cCla;
                char t_buffer [100];
                const char * t_claPathx = 0;
                wsprintf (t_buffer, "%i",cCla);
                t_claPathx = t_buffer;
				TiXmlAttribute* attribute;
				const char* name;
				const char* value;
				for (attribute = element->FirstAttribute() ; attribute ; attribute = attribute->Next()) 
				{ 
					name = attribute->Name(); 
					value = attribute->Value();
					if (0==lstrcmp(name, "name")) continue;
					this->template_vars[name] = value;
				} 
				std::map<const char*,const char*>::iterator iter;   
				for( iter = this->template_vars.begin(); iter != this->template_vars.end(); iter++ ) {
					Dcx::mIRC.execex("//set %%%s %s",iter->first,iter->second);
				}
                templateRefclaPath = t_claPathx;
                this->parseTemplate(depth,claPath,passedid);
                templateRef = 0;
				for( iter = this->template_vars.begin(); iter != this->template_vars.end(); iter++ ) {
					Dcx::mIRC.execex("//unset %%%s",iter->first);
				}
				this->template_vars.clear();
            }
            continue;
        }
		if ((0==lstrcmp(elem, "control")) || (0==lstrcmp(elem, "pane"))) cCla++;
        else continue;

        //asign ID 
        if (0==lstrcmp(elem, "control")) 
		{ 
            controls++;
            id = this->parseId(element);
			id = (id > 0) ? id : 2000 + controls;
			this->registerId(element,id);
			this->registerId(element,id);
        }
        else id = passedid;

        //assign parent CONTROL of element
        while (parent) {
            if (0==lstrcmp(parentelem, "template")) 
            {
                parent = templateRef->Parent()->ToElement();
                parentelem = templateRef->Parent()->Value();
                cCla = templateRefcCla;
                claPath = templateRefclaPath;
            }
            if (0==lstrcmp(parentelem, "pane")) 
            { 
                parent = parent->Parent()->ToElement();
                parentelem = parent->Value();
            }
            else break;    
        }
        parenttype = (temp = parent->Attribute("type")) ? temp : "panel";
        parentid = this->parseId(parent);
		parentid = (parentid > 0) ? parentid : passedid;
        //IF TEMPLATE ELEMENT REROUTE TO TEMPLATE DEFINITION


        //STEP 3: IF CONTROL CREATE IT AND ITS ITEMS
		if (0==lstrcmp(elem, "control")) {

            control++;
			
            //check how to insert the control in the parent Control/Dialog
            //If parentNode is pane loop untill parentNode is not a pane
            if (0==lstrcmp(parentelem, "dialog")) { 
                xdialogEX("-c","%i %s 0 0 %s %s %s",id,type,width,height,styles);
            }
            else if (0==lstrcmp(parentelem, "control")) { 
                if (0==lstrcmp(parenttype, "panel"))
                    this->xdidEX(parentid,"-c","%i %s 0 0 %s %s %s",
                        id,type,width,height,styles);

                else if (0==lstrcmp(parenttype, "box"))
                    this->xdidEX(parentid,"-c","%i %s 0 0 %s %s %s",
                        id,type,width,height,styles);

                else if (0==lstrcmp(parenttype, "tab"))
                    this->xdidEX(parentid,"-a","0 %s %s \t %i %s 0 0 %s %s %s \t %s",
                        icon,caption,id,type,width,height,styles,tooltip);

                else if (((0==lstrcmp(parenttype, "pager")) || (0==lstrcmp(parenttype, "box"))) && (control == 1))
					this->xdidEX(parentid,"-c","%i %s 0 0 %s %s %s",
                        id,type,width,height,styles);

                else if (0==lstrcmp(parenttype, "divider") && (control <= 2)) {
                        if (control == 1)
                            this->xdidEX(parentid,"-l","%s 0 \t %i %s 0 0 %s %s %s",
                                width,id,type,width,height,styles);
                        else if (control == 2)
                            this->xdidEX(parentid,"-r","%s 0 \t %i %s 0 0 %s %s %s",
                                width,id,type,width,height,styles);
                }

                else if (0==lstrcmp(parenttype, "rebar")) { 
                    const char *flags = (tFlags) ? tFlags : "ceg";
                    this->xdidEX(parentid,"-a","0 +%s %s %s %s %s %s %s \t %i %s 0 0 %s %s %s \t %s",
                        flags,rebarMinWidth,rebarMinHeight,width,icon,textcolour,caption,
                        id,type,width,height,styles,tooltip);
                }
                else if (0==lstrcmp(parenttype, "stacker")) 
                    this->xdidEX(parentid,"-a","0 + %s %s %s \t %i %s 0 0 %s %s %s",
                        textcolour,bgcolour,caption,id,type,width,height,styles);
                
                else if (0==lstrcmp(parenttype, "statusbar"))
                    this->xdidEX(parentid,"-t","%i +c %s %i %s 0 0 0 0 %s",
                        cell,icon,id,type,styles);
            }
            
            
        }
        //Set CLA for control or pane
        g_claPath = claPath;
        TString claPathx = this->parseCLA(cCla);
		
        //Perform some control specific commands
        if (0==lstrcmp(elem, "control")) {
            this->parseControl();
            this->parseIcons();	
            this->parseStyle();
			
        }
        //char *claPathx = "root";
        //Dcx::mIRC.execex("//echo -a clapath:%s",claPathx);
        this->parseDialog(depth+1,claPathx.to_chr(),id,0);  
    }
} 


int DcxmlParser::mIRCEvalToUnsignedInt (const char *value)
	{
		//Todo: method returns -1 for failure which odd for a *ToUnsignedInt method.
		TString buf((UINT)32);
		Dcx::mIRC.evalex(buf.to_chr(), 32, value,"");
		int id = buf.to_int();
		return (id > 0) ? id : -1;
	}
void DcxmlParser::registerId(TiXmlElement *idElement,int id)
	{
		int elementId;
		if (idElement->QueryIntAttribute("id",&elementId) != TIXML_SUCCESS) //<! id attr. is not an int
		{
			const char *elementNamedId = (temp = idElement->Attribute("id")) ? temp : "";
			if (this->mIRCEvalToUnsignedInt(elementNamedId) < 0) //<! id attr. doesn't evaluate to an int
			{
				this->getDialog()->namedIds[elementNamedId] = id;
			}
		}
	}
int DcxmlParser::parseId(TiXmlElement* idElement)
	{
		//<! if id attribute is already integer return it
		int id = (idElement->QueryIntAttribute("id",&id) == TIXML_SUCCESS) ? id : 0;
		if (id > 0) return id;
		
		//<! else try to evaluate the value to see if it ends up as an id;
		const char *attributeIdValue = (temp = idElement->Attribute("id")) ? temp : "0";
		id = mIRCEvalToUnsignedInt(attributeIdValue);
		if (id > 0) return id;
		
		TString value(attributeIdValue);

		//Otherwise if it's a namedId return it .find(attributeIdValue) never returned :(;
		for(IntegerHash::const_iterator it = this->getDialog()->namedIds.begin(); it != this->getDialog()->namedIds.end(); ++it)
			if (it->first == value) return it->second;	

		return 0;
	}
