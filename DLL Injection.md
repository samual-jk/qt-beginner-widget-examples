## **Title**

**Security Risk: OpenSSL Configuration & Module Injection via Environment Variables**
https://medium.com/@ishanchowdhury2018/dll-hijacking-analysis-using-process-monitor-6e6d60c0ab6b
---

## **Current Behaviour**

Our application currently uses OpenSSL without restricting or overriding its default behavior.

By design, OpenSSL automatically:

* Loads configuration from external files (e.g., `openssl.cnf`)
* Reads environment variables such as:

  * `OPENSSL_CONF`
  * `OPENSSL_MODULES`
  * `OPENSSL_ENGINES`
* Dynamically loads external modules (DLLs) based on these configurations

Since our application does **not disable or override these paths**, it implicitly trusts:

* System environment variables
* User-controlled directories
* External configuration files

This creates a situation where **external factors (outside the application) can influence cryptographic behavior at runtime**.

---

## **Expected Behaviour**

The application should operate in a **fully controlled and deterministic cryptographic environment**, where:

* OpenSSL does **not load any external configuration files**
* No external modules or engines are dynamically loaded unless explicitly intended
* All cryptographic behavior is defined strictly within the application

This ensures:

* No dependency on user/system environment
* No possibility of external tampering
* Consistent and secure execution across all systems

---

## **Possible Fix**

Override OpenSSL-related environment variables at application startup to prevent loading of external configuration and modules.

### Recommended implementation:

```cpp
#ifdef Q_OS_WIN
qputenv("OPENSSL_CONF", "::");
qputenv("OPENSSL_MODULES", "::");
qputenv("OPENSSL_ENGINES", "::");
#endif
```

### Explanation:

* `"::"` is an invalid path
* Forces OpenSSL to **fail loading any external config or modules**
* Ensures application runs in an isolated and controlled mode

### Additional (optional but stronger):

* Explicitly disable config loading using OpenSSL APIs:

  ```cpp
  OPENSSL_init_crypto(OPENSSL_INIT_NO_LOAD_CONFIG, nullptr);
  ```

---

## **Impact**

### 🔴 Severity: High (Critical in sensitive applications)

### Technical Impact:

* External configuration can alter cryptographic behavior
* Unauthorized dynamic loading of DLLs (engines/modules)
* Potential execution of malicious code within application context

### Business Impact:

* Compromise of sensitive data (passwords, tokens, encryption keys)
* Loss of customer trust
* Compliance violations (especially in security-sensitive domains)
* Potential financial and reputational damage

### Operational Impact:

* Non-deterministic behavior across machines
* Difficult-to-debug production issues due to environment differences

---

## **How an Attacker Can Exploit This**

### Step-by-step attack scenario:

1. **Prepare malicious environment**

   * Attacker sets environment variable:

     ```
     OPENSSL_ENGINES=C:\malicious_folder
     ```

2. **Place malicious DLL**

   * A crafted DLL is placed inside that folder

3. **Application launch**

   * User launches the application normally

4. **OpenSSL initialization**

   * OpenSSL reads environment variables
   * Automatically loads engine/module from attacker-controlled path

5. **Malicious code execution**

   * DLL gets loaded into application process
   * Attacker gains execution inside the app

---

## **What the Attacker Gains**

### 🔓 Possible Capabilities:

* **Code Execution**

  * Run arbitrary code within application process

* **Data Exfiltration**

  * Access sensitive data (passwords, encryption keys, user data)

* **Cryptographic Manipulation**

  * Alter encryption/decryption behavior
  * Weaken security silently

* **Persistence**

  * Maintain hidden access via injected modules

* **Privilege Escalation (in some cases)**

  * If application runs with elevated privileges

---

## **Summary (For Non-Technical Audience)**

Currently, the application allows external system settings to influence how encryption works internally.
This means a malicious actor could trick the application into loading harmful code without modifying the application itself.

By applying a simple fix, we can:

* Prevent external interference
* Ensure consistent and secure behavior
* Eliminate a critical attack vector

---

## **Recommendation**

This issue should be treated as a **high-priority security hardening task** and addressed in the next release cycle.

---
