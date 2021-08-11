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
  (file-name-directory (buffer-file-name))
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

(setq-local org-export-with-section-numbers nil
      org-export-with-smart-quotes t
      org-export-with-toc nil)

(setq-local org-html-divs '((preamble "header" "top")
                      (content "main" "content")
                      (postamble "footer" "postamble"))
      org-html-container-element "section"
      org-html-metadata-timestamp-format "%Y-%m-%d"
      org-html-checkbox-type 'html
      org-html-html5-fancy t
      org-html-validation-link nil
      org-html-doctype "html5")

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
             :publishing-directory "./public"
             :publishing-function 'org-publish-attachment
             :recursive t)
       (list "site" :components '("site-org"))))

(provide 'publish)
;;; publish.el ends here
