;; publish.el --- Publish org-mode project on Gitlab Pages
;; Original Author: Rasmus

;;; Commentary:
;;;
;;; Taken from Rasmus and edited for my purposes. I downloaded the original
;;; from: https://gitlab.com/pages/org-mode/-/raw/master/publish.el

;;; Code:

(require 'org)
(require 'ox-publish)

(defvar-local current-directory
  (file-name-directory (or load-file-name (buffer-file-name)))
  "The full path of the current directory.")
(defvar-local publish-directory
  (expand-file-name "public"
                    (file-name-directory (directory-file-name current-directory)))
  "The path be used as the export directory.")
(defvar-local site-attachments
  (regexp-opt '("jpg" "jpeg" "gif" "png" "svg"
                "ico" "cur" "css" "js" "woff" "html" "pdf"))
  "File types that are published as static files.")

;; setting to nil, avoids "Author: x" at the bottom
(setq-local user-full-name nil)

(setq-local org-export-default-language "en"
            org-export-with-section-numbers nil
            org-export-with-smart-quotes t
            org-export-with-emphasize t
            org-export-with-special-strings t
            org-export-with-fixed-width t
            org-export-with-timestamps nil
            org-export-preserve-breaks t
            org-export-with-sub-superscripts t
            org-export-with-archived-trees nil
            org-export-with-author nil
            org-export-with-broken-links t
            org-export-with-clocks nil
            org-export-with-creator nil
            org-export-with-date t
            org-export-with-entities t
            org-export-with-email nil
            org-export-with-footnotes t
            org-export-headline-levels  3
            org-export-with-inlinetasks nil
            org-export-with-planning nil
            org-export-with-priority nil
            org-export-with-properties nil
            org-export-with-tags nil
            org-export-with-tasks nil
            org-export-time-stamp-file nil
            org-export-with-title t
            org-export-with-toc nil
            org-export-with-todo-keywords nil
            org-export-with-tables t)

(setq-local org-html-metadata-timestamp-format "%m-%d-%Y"
            org-html-checkbox-type 'html
            org-html-html5-fancy t
            org-html-validation-link nil
            org-html-doctype "html5"
            org-html-use-infojs t
            org-html-infojs-options '((path . "org-info.js")
                                      (view . "info")
                                      (toc . :with-toc)
                                      (ftoc . "0")
                                      (tdepth . "max")
                                      (sdepth . "max")
                                      (mouse . "underline")
                                      (buttons . "0")
                                      (ltoc . "1")
                                      (up . :html-link-up)
                                      (home . :html-link-home)))

(setq-local org-publish-project-alist
            (list
             (list "site-org"
                   :base-directory current-directory
                   :base-extension "org"
                   :recursive t
                   :publishing-function '(org-html-publish-to-html)
                   :publishing-directory publish-directory
                   :exclude (regexp-opt '("README" "draft"))
                   :auto-sitemap t
                   :sitemap-filename "index.org"
                   :sitemap-file-entry-format "%d *%t*"
                   :html-head-extra "<link rel=\"icon\" type=\"image/x-icon\" href=\"/favicon.ico\"/>"
                   :sitemap-style 'list
                   :sitemap-sort-files 'anti-chronologically)
             (list "site-static"
                   :base-directory "."
                   :exclude publish-directory
                   :base-extension site-attachments
                   :publishing-directory publish-directory
                   :publishing-function 'org-publish-attachment
                   :recursive t)
             (list "site" :components '("site-org"))))

(provide 'publish)
;;; publish.el ends here
