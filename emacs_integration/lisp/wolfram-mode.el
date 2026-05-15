;;; wolfram-mode.el -*- lexical-binding: t; -*-


;;; Commentary: Major mode for wolfram language


;;; Code:
;; use 'smie' to setup AST
(require 'smie)
(require 'fontlock-wolfram)


;; customs Variables
(defgroup wolfram-mode nil
  "Editing Wolfram Language code"
  :group 'languages)

(defcustom wolfram-mode-hook nil
  "Normal hook run when entering `wolfram-mode'.
See `run-hooks'."
  :type 'hook
  :group 'wolfram-mode)

(defcustom wolfram-indent-offset 8
  "Basic Indentation for newline."
  :type 'integer
  :group 'wolfram-mode)

(defcustom wolfram-kernel-bin
  "/usr/local/Wolfram/Wolfram/14.3/SystemFiles/Kernel/Binaries/Linux-x86-64/WolframKernel"
  "The place where the binary - 'Wolframkernel' lies."
  :type 'string
  :group 'wolfram-mode)


;; setup wolfram language AST parser(for 'indent-line-function', grammar, etc)
(defvar wolfram-outline-regexp "\\((\\*\\|.+?:=\\)")

(defvar wolfram-smie-grammar
  (smie-prec2->grammar
   (smie-bnf->prec2
    `((head) (epsilon) (string)
      (expr (head "[" exprs "]")
            (expr "[[" exprs "]]")
            ("{" exprs "}")
            ("(" expr ")")
	    ("<|" exprs "|>")
            ;; message
            (expr "::" string)
            ;; statement separation
            (expr ";" expr)
            (expr "&")
            ;; delayed set
            (expr ":=" expr)
            (head "/:" expr ":=" expr)
            ;; set
            (expr "=" expr)
            (head "/:" expr "=" expr)
            (expr "+=" expr)
            (expr "-=" expr)
            (expr "*=" expr)
            (expr "/=" expr)
            ;; operation
            (expr "~" head "~" expr)
            (expr "@@" expr)
            (expr "==" expr)
            (expr "||" expr)
            (expr "&&" expr)
            (expr "+" expr)
            (expr "-" expr)
            (expr "*" expr)
            (expr "/" expr)
            (expr "^" expr)
            ;; application
            (expr ":" expr)
            (expr "/;" expr)
            (expr "//" expr))
      (exprs (epsilon)
             (expr)
             (exprs "," expr)))
    '((assoc ";")
      (assoc "::")
      (assoc "&")
      (assoc "/:")
      (assoc ":=" "=" "+=" "-=" "*=" "/=")
      (assoc "/;" ":" "//")
      (assoc "~")
      (assoc "@@" "==")
      (assoc "||" "&&")
      (assoc "+" "-")
      (assoc "*" "/")
      (assoc "^")
      (assoc "[[")))))

(defun wolfram-smie-rules (kind token)
  "Wolfram Language SMIE indentation function for KIND and TOKEN."
  (pcase (cons kind token)
    (`(:before . "[")
     (save-excursion
       (smie-default-backward-token)
       `(column . ,(current-column))))
    (`(:after . ":=") `(column . ,wolfram-indent-offset))
    (`(:after . ,(or "]" "}" ")" "|>")) '(column . 0))
    (`(:after . ,(or "[" "{" "(" "<|"))
     (save-excursion
       (beginning-of-line)
       (skip-chars-forward " \t")
       `(column . ,(+ wolfram-indent-offset (current-column)))))
    (`(,_ . ";") (smie-rule-separator kind))
    (`(,_ . ",") (smie-rule-separator kind))
    (`(:elem . ,_) 0)
    (_ nil)))

(defalias 'wolfram-smie-forward-token  'smie-default-forward-token)
(defalias 'wolfram-smie-backward-token 'smie-default-backward-token)


;; LSP for wolfram language - 'lsp-mode' & 'WolframKernel'
;; FIXME: 'lsp-mode' will raise error 'Company: backend company-capf error "Wrong type argument:
;;        sequencep, #s(hash-table test equal)" with args (candidates -3)'
(with-eval-after-load 'lsp-mode
  (add-to-list 'lsp-language-id-configuration '(wolfram-mode . "wolfram-lsp"))
  (lsp-register-client
   (make-lsp-client
    :new-connection (lsp-stdio-connection
         `(,wolfram-kernel-bin
           "-noinit" "-noprompt" "-nopaclet" "-noicon" "-nostartuppaclets"
           "-run" "Needs[\"LSPServer`\"];LSPServer`StartServer[]"))
    :major-modes '(wolfram-mode)
    :server-id 'wolfram-lsp))
)
;; supporess company error messages in minibuffer
;; FIXME: does not work
;; (defun suppress-messages (old-fun &rest args)
;;   (cl-flet ((silence (&rest args1) (ignore)))
;;     (advice-add 'message :around #'silence)
;;     (unwind-protect
;;       (apply old-fun args)
;;       (advice-remove 'message #'silence))
;;   )
;; )
;; (advice-add 'company-call-backend-raw :around #'suppress-messages)


;; LSP for wolfram language - 'eglot' & 'WolframKernel'
(with-eval-after-load 'eglot
  (add-to-list 'eglot-server-programs
    `(wolfram-mode . (,wolfram-kernel-bin
      "-noinit" "-noprompt" "-nopaclet" "-noicon" "-nostartuppaclets"
      "-run" "Needs[\"LSPServer`\"];LSPServer`StartServer[]"))))


;;;###autoload
(define-derived-mode wolfram-mode prog-mode "Wolfram Language"
  "Major mode for editing Wolfram Language files in Emacs, compatible with the Wolfram LSP server.

\\{wolfram-mode-map}
Entry to this mode calls the value of `wolfram-mode-hook'
if that value is non-nil."
  :syntax-table wolfram-mode-syntax-table
  :abbrev-table wolfram-mode-abbrev-table
  (smie-setup wolfram-smie-grammar #'wolfram-smie-rules
              :forward-token  'wolfram-smie-forward-token
              :backward-token 'wolfram-smie-backward-token)
  (wolfram-mode-variables)
)

(add-to-list 'auto-mode-alist '("\\.wl\\'"  . wolfram-mode))
(add-to-list 'auto-mode-alist '("\\.wls\\'" . wolfram-mode))

(defun wolfram-mode-variables ()
  "Local variables for both Major and Inferior mode."
  (set-syntax-table wolfram-mode-syntax-table)
  ;; set local variables
  (setq-local comment-start "(*")
  (setq-local comment-end   "*)")
  (setq-local comment-start-skip "(\\*")
  (set (make-local-variable 'syntax-propertize-function)
       wolfram-syntax-propertize-function)
  (setq-local font-lock-defaults '(wolfram-font-lock-keywords nil nil))
  (setq-local outline-regexp wolfram-outline-regexp)
)


(provide 'wolfram-mode)
;;; wolfram-mode.el ends here
