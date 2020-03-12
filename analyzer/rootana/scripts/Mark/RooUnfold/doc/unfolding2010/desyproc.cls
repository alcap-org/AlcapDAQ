\NeedsTeXFormat{LaTeX2e}
\ProvidesClass{desyproc}[2009/01/25 V1.0 DESY proceedings]

\DeclareOption*{\PassOptionsToClass{\CurrentOption}{article}}
\ProcessOptions\relax
\LoadClass[twoside]{article}
\usepackage{url}
\usepackage[dvips]{graphicx,epsfig,color}
\usepackage{wrapfig,rotating}
\usepackage{amssymb,amsmath,array}

\setlength{\textwidth}{145mm}
\setlength{\textheight}{198mm}
\setlength{\topmargin}{5mm}
\setlength{\oddsidemargin}{6mm}
\setlength{\evensidemargin}{8mm}


\newif\if@doi\@doifalse
\def\contribID#1{\gdef\@contribID{#1}}
\def\@contribID{\@latex@warning{Please provide the Contribution-ID}}
\def\confID#1{\@doitrue\gdef\@confID{#1}}
\def\doi{\@doitrue}

\def\desyproc#1{\gdef\@desyproc{#1}}                       
\gdef\@desyproc{\@latex@warning{No DESY-PROC number given}}

\def\acronym#1{\gdef\@acronym{#1}}                                  
\let\@acronym\@empty                                     


\def\@maketitle{%
  \newpage
  \parindent 0mm
  \null
  \vskip 2em%
  \let \footnote \thanks
    {\bfseries\LARGE \@title \par}%
    \vskip 1.5em%
    {\lineskip .5em%
        \@author
    }%
  \if@doi
     \par
     \vskip 1em
% =======  To the editor:  ==========================================
%   Please swap the comment of the following 2 lines. 
%   So the propper DOI appears on the document.
% -------------------------------------------------------------------
%     {\bfseries DOI:} {\texttt http://dx.doi.org/10.3204/\@desyproc/\@contribID}
     {\bfseries DOI:} {\texttt will be assigned}
  \fi
  \par
  \vskip 1.5em}

\if@titlepage
\else
  \renewenvironment{abstract}{%
      \if@twocolumn
        \section*{\abstractname}%
      \else
        \small
        \quote
      \fi}
      {\if@twocolumn\else\endquote\fi}
\fi

\def\ps@plain{\let\@mkboth\@gobbletwo
     \let\@oddhead\@empty\let\@evenhead\@empty
     \def\@evenfoot{\thepage\hfil\slshape \@acronym}%
     \def\@oddfoot{{\slshape \@acronym}\hfil\thepage}%
     }
\def\ps@empty{\let\@mkboth\@gobbletwo
     \let\@oddhead\@empty\let\@evenhead\@empty
     \def\@evenfoot{\hfil\slshape \@acronym}%
     \def\@oddfoot{{\slshape \@acronym}\hfil}%
}
\pagestyle{plain}
