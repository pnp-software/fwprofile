\NeedsTeXFormat{LaTeX2e}
\ProvidesClass{fwprofile}[2026/09/28 Shared FW Profile document class]

\newif\iffwdefinition
\newif\iffwmanual
\newif\iffwfaq
\newif\iffwrequirements
\DeclareOption{definition}{\fwdefinitiontrue}
\DeclareOption{manual}{\fwmanualtrue}
\DeclareOption{faq}{\fwfaqtrue}
\DeclareOption{requirements}{\fwrequirementstrue}
\DeclareOption*{\PassOptionsToClass{\CurrentOption}{article}}
\ProcessOptions\relax
\LoadClass[a4paper,10pt]{article}

\RequirePackage[utf8x]{inputenc}
\RequirePackage{array}
\RequirePackage[pdftex]{graphicx}
\RequirePackage{hyperref}
\RequirePackage{textcomp}
\RequirePackage{listings}
\RequirePackage{color, colortbl}
\RequirePackage{longtable}
\RequirePackage{afterpage}
\RequirePackage{enumitem}
\RequirePackage{float}
\RequirePackage{fancyvrb}
\RequirePackage{fancyhdr}
\RequirePackage{pdflscape}
\RequirePackage[compact]{titlesec}
\RequirePackage{etoolbox}
\RequirePackage{verbatim}
\RequirePackage[labelfont=bf]{caption}
\iffwrequirements
  \RequirePackage{parskip}
\fi

% Compatibility fix for titlesec 2.10.1.
\makeatletter
\patchcmd{\ttlh@hang}{\parindent\z@}{\parindent\z@\leavevmode}{}{}
\patchcmd{\ttlh@hang}{\noindent}{}{}{}
\makeatother

\pagestyle{fancy}
\setlength{\parindent}{0in}
\setlength{\abovecaptionskip}{0pt}
\renewcommand{\figurename}{Fig.}
\renewcommand{\headrulewidth}{0.4pt}
\renewcommand{\footrulewidth}{0.4pt}

\let\stdsection\section
\renewcommand\section{\newpage\stdsection}
\newcommand{\fwbody}{\setlength{\parskip}{3mm}}
\newcommand{\fwbreakbeforesubsections}{%
  \let\stdsubsection\subsection
  \renewcommand\subsection{\newpage\stdsubsection}%
}

\iffwdefinition
  \setlength{\abovecaptionskip}{2pt}
  \setlength{\belowcaptionskip}{-8pt}
  \captionsetup[table]{position=bottom,skip=-1pt}
  \setlength{\extrarowheight}{1.5pt}
  \titlespacing\subsubsection{0pt}{5pt}{0pt}
  \titlespacing\subsection{0pt}{5pt}{0pt}
\fi
\iffwfaq
  \titlespacing\subsection{0pt}{5pt}{-3pt}
  \titleformat*{\subsection}{\normalsize\bfseries}
\fi
\iffwmanual
  \graphicspath{{../images/}}
\fi
\iffwrequirements
  \captionsetup[table]{singlelinecheck=off,justification=raggedright}
\fi

\setlist{nolistsep}
\newenvironment{fw_itemize}
  {\begin{itemize}\setlength{\itemsep}{1mm}\setlength{\parskip}{0pt}\setlength{\parsep}{0pt}}
  {\end{itemize}}
\newenvironment{fw_enumerate}
  {\begin{enumerate}\setlength{\itemsep}{1mm}\setlength{\parskip}{0pt}\setlength{\parsep}{0pt}}
  {\end{enumerate}}
\newenvironment{fw_description}
  {\begin{description}\setlength{\itemsep}{1mm}\setlength{\parskip}{5pt}\setlength{\parsep}{0pt}}
  {\end{description}}

\newcommand{\fwheader}[2]{%
  \lhead{#1}%
  \chead{}%
  \rhead{Revision #2}%
  \lfoot{\textcopyright2012 P\&P Software GmbH. All Rights Reserved.}%
  \cfoot{}%
  \rfoot{\thepage}%
}
\newcommand{\fwlicensedto}{%
  \cfoot{\vspace{5mm}{\color{red}\verbatiminput{../commercial/LicensedTo.txt}}}%
}
\newcommand{\fwsetpdfmetadata}[2]{%
  \hypersetup{%
    pdfauthor={Alessandro Pasetti, Vaclav Cechticky},%
    pdfsubject={#1},%
    pdftitle={#2},%
    pdfkeywords={Framework, UML, C-language, State Machine, Activity Diagrams, Embedded, Realtime}%
  }%
}
\newcommand{\fwdocument}[5]{%
  \fwheader{#1}{#2}%
  \iffwdefinition\else\fwlicensedto\fi
  \title{#4}%
  \author{Alessandro Pasetti \& Vaclav Cechticky}%
  \date{#5}%
  \def\fwtitleRevision{#3}%
  \def\fwdocumentCode{#1}%
}
\newcommand{\fwseparator}{%
  \begin{table}[ht]
  \begin{center}
  \begin{tabular}{p{11.7cm}}
  \\
  \hline
  \end{tabular}
  \end{center}
  \end{table}%
}
\newcommand{\fwmaketitle}{%
  \maketitle
  \begin{center}
  Revision \fwtitleRevision \\
  \fwdocumentCode
  \end{center}
  \iffwdefinition\vspace{1cm}\fi
  \iffwfaq\vspace{1cm}\fi
  \begin{center}
  P\&P Software GmbH \\
  High Tech Center 1 \\
  8274 T\"{a}gerwilen \\
  Switzerland \\
  \vspace{2mm}
  Web site: \url{www.pnp-software.com}\\
  E-mail: \href{mailto:pnp-software@pnp-software.com}{\nolinkurl{pnp-software@pnp-software.com}}
  \end{center}
  \iffwdefinition\vspace{1.2cm}\fi
  \iffwfaq\vspace{0.5cm}\fi
  \fwseparator
}
\newcommand{\fwcopyrightpage}[1]{%
  \newpage
  \vspace*{\fill}
  \begin{center}
  No part of this publication may be reproduced, transmitted, transcribed, stored in any retrieval system, or translated into any language
  by any means without express prior written permission of P\&P Software GmbH.
  \end{center}
  \begin{center}
  Copyright \textcopyright #1 P\&P Software GmbH. All Rights Reserved.
  \end{center}
  \vspace*{\fill}%
}
\newcommand{\fwcontentsandlists}{%
  \newpage
  \tableofcontents
  \newpage
  \listoffigures
  \listoftables
  \lstlistoflistings
}
\newcommand{\fwfrontmatter}{%
  \fwseparator
  \iffwdefinition
    \fwcopyrightpage{2012}%
    \fwcontentsandlists
  \fi
  \iffwmanual
    \fwcopyrightpage{2013}%
    \fwcontentsandlists
  \fi
  \iffwfaq
    \fwcopyrightpage{2012}%
    \newpage
    \tableofcontents
    \newpage
  \fi
  \iffwrequirements
    \fwcopyrightpage{2012}%
    \fwcontentsandlists
  \fi
  \fwbody
}

\definecolor{dkgreen}{rgb}{0,0.6,0}
\definecolor{gray}{rgb}{0.5,0.5,0.5}
\definecolor{mauve}{rgb}{0.58,0,0.82}
\definecolor{lightblue}{RGB}{128,179,255}
\definecolor{lbcolor}{rgb}{0.9,0.9,0.9}
\definecolor{light-gray}{gray}{0.85}

\newcommand{\fwlistingbackground}{lbcolor}
\newcommand{\fwlistingstyle}{\footnotesize\ttfamily}
\iffwdefinition
  \renewcommand{\fwlistingbackground}{white}
  \renewcommand{\fwlistingstyle}{\footnotesize}
\fi
\lstset{
  language=C,
  aboveskip=\bigskipamount,
  basicstyle=\fwlistingstyle,
  numbers=left,
  numberstyle=\tiny\color{gray},
  stepnumber=1,
  numbersep=5pt,
  backgroundcolor=\color{\fwlistingbackground},
  showspaces=false,
  showstringspaces=false,
  showtabs=false,
  frame=single,
  rulecolor=\color{black},
  tabsize=2,
  captionpos=b,
  breaklines=true,
  breakatwhitespace=false,
  title=\lstname,
  keywordstyle=\color{blue},
  commentstyle=\color{dkgreen},
  stringstyle=\color{mauve},
  escapeinside={\%*}{*)},
  morekeywords={*,...}
}

\iffwrequirements
  \renewcommand{\arraystretch}{1.5}
  \newcolumntype{P}[1]{>{%
    \parskip=0.5\baselineskip%
    \advance\parskip by 0pt plus 2pt
    \setlength{\parfillskip}{30pt plus 1fil}}p{#1}}
  \newenvironment{fw_req}[6]
    {\addtocounter{subsubsection}{1}
      \hspace{0.2cm}\textbf{FW-\arabic{section}.\arabic{subsection}.\arabic{subsubsection}/#2
      \hspace{0.8cm} #1}
      \vspace{-10pt}
      \begin{longtable}{p{2.7cm}P{8.5cm}}
      \hline
      \textsc{Requirement} & #3 \\
      \textsc{Justification} & #4 \\
      \textsc{Implementation} & #5 \\
      \textsc{Verification} & #6 \\
      \hline}
    {\end{longtable}}
  \newenvironment{fw_req_note}[7]
    {\addtocounter{subsubsection}{1}
      \hspace{0.2cm}\textbf{FW-\arabic{section}.\arabic{subsection}.\arabic{subsubsection}/#2
      \hspace{0.8cm} #1}
      \vspace{-10pt}
      \begin{longtable}{p{2.7cm}P{8.5cm}}
      \hline
      \textsc{Requirement} & #3 \\
      \textsc{Note} & #4 \\
      \textsc{Justification} & #5 \\
      \textsc{Implementation} & #6 \\
      \textsc{Verification} & #7 \\
      \hline}
    {\end{longtable}}
\fi

\endinput