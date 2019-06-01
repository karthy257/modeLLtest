# modeLLtest [![Build Status](https://travis-ci.com/ShanaScogin/modeLLtest.svg?branch=master)](https://travis-ci.com/ShanaScogin/modeLLtest)
An R Package which implements model comparison tests using cross-validated log-likelihood (CVLL) values. 

# Test Overviews
An R package which implements model comparison tests. `modeLLtest` includes functions for the cross-validated difference in means (CVDM) test and the cross-validated median fit (CVMF) test. Other tools include a function to output a vector of cross-validated log-likelihood (CVLL) values and a function to perform the CVDM test on an input of two CVLL vectors. The relevant papers including details can be found below:

* Harden, J. J., & Desmarais, B. A. (2011). Linear Models with Outliers: Choosing between Conditional-Mean and Conditional-Median Methods. State Politics & Policy Quarterly, 11(4), 371-389.

* Desmarais, B. A., & Harden, J. J. (2012). Comparing partial likelihood and robust estimation methods for the Cox regression model. Political Analysis, 20(1), 113-135.

* Desmarais, B. A., & Harden, J. J. (2014). An unbiased model comparison test using cross-validation. Quality & Quantity, 48(4), 2155-2173.

# Installing The Package
Currently the package can be downloaded with the devtools package in R from GitHub. To do this, install devtools by calling:

```
install.packages("devtools")
```

Now we can install from GitHub with the following line:

```
devtools::install_github("ShanaScogin/modeLLtest")
```

Once you have installed the package, you can access it by calling:

```
library(modeLLtest)
```

After the package is loaded, check out the `?modeLLtest` to see a help file. You can also see the documentation for the functions with `?cvdm`, `?cvll`, `?cvlldiff`, or `?cvmf`. If you have issues or questions, please email me at sscogin@nd.edu.

# Basic Usage
This package has four main functions: `cvdm()`, `cvll()`, `cvmf()`, and `cvlldiff()`. The function `cvdm()` deploys the CVDM test, which uses a bias-corrected Johnson's t-test to choose between the leave-one-out cross-validated log-likelihood outputs of two non-nested models. The function `cvll()` outputs a vector of leave-one-out cross-validated log-likelihoods for a given method. Currently, these functions accommodate linear regression, median regression (from the package `quantreg`), and two methods of robust regression (from the package `MASS`). The`cvlldiff()` function performs the bias-corrected Johnson's t-test on two vectors of cross-validated log-likelihoods. Finally, the `cvmf()` function test between the partial likelihood maximization (PLM) and the iteratively reweighted robust (IRR) methods of estimation for a given application of the Cox model

## Examples
Here are some examples of the functions in this package. First, we'll look at `cvdm()`, which applies cross-validated log-likelihood difference in means (CVDM) test to compare two methods of estimating a formula. The example compares ordinary least squares (OLS) estimation to median regression (MR).

```
library(modeLLtest)
set.seed(123456)
b0 <- .2 # True value for the intercept
b1 <- .5 # True value for the slope
n <- 500 # Sample size
X <- runif(n, -1, 1)

Y <- b0 + b1 * X + rnorm(n, 0, 1) # N(0, 1 error)

obj_cvdm <- cvdm(Y ~ X, data.frame(cbind(Y, X)), method1 = "OLS", method2 = "MR")

obj_cvdm
  
```

Next, let's do the same as we did above, but with `cvll()` and `cvlldiff()`. These are general functions that create vectors of the cross-validated log-likelihood functions and then compute the bias-corrected Johnson's t-test, respectively.

```
library(modeLLtest)
set.seed(123456)
b0 <- .2 # True value for the intercept
b1 <- .5 # True value for the slope
n <- 500 # Sample size
X <- runif(n, -1, 1)

Y <- b0 + b1 * X + rnorm(n, 0, 1) # N(0, 1 error)

obj_cvll_ols <- cvll(Y ~ X, data.frame(cbind(Y, X)), method = "OLS")

obj_cvll_mr <- cvll(Y ~ X, data.frame(cbind(Y, X)), method = "MR")

obj_cvlldiff <- cvlldiff(obj_cvll_ols$cvll, obj_cvll_mr$cvll,
                           obj_cvll_ols$df)

obj_cvlldiff
```

Finally, let's look at the `cvmf()` function. This function compares the partial likelihood maximization (PLM) and the iteratively reweighted robust (IRR) methods of estimation for a given application of the Cox model.

```
library(modeLLtest)
library(survival)

set.seed(12345)
x1 <- rnorm(100) 
x2 <- rnorm(100)
x2e <- x2 + rnorm(100, 0, 0.5)

y <- rexp(100, exp(x1 + x2))
y <- Surv(y) # Changing y into a survival obj with survival package

dat <- data.frame(y, x1, x2e)
form <- y ~ x1 + x2e

obj_cvmf <- cvmf(formula = form, data = dat)

obj_cvmf
```

# Examples with Replication Data
For an example using real-world analysis, we can look at a study by Joshi and Mason (2008, Journal of Peace Research 45(6): 765-782) that employs robust regression to analyze district-level election turnout among peasants in Nepal. Specifically, they hypothesize that peasant dependence on landed elite for survival will result in higher voter turnout. In their model of the 1999 parliamentary elections below, we can see how their use robust regression is supported by the CVDM test. These data are available on the [Journal of Peace Research Replication Datasets website](https://www.prio.org/JPR/Datasets/) and have been included in the package for ease of replication. For full replication and discussion of the CVDM test, see Desmarais and Harden (2014, Quality and Quantity 48(4): 2155-2173).

```
library(MASS)
library(modeLLtest)

data(nepaldem)

obj_cvdm_jm <- cvdm(percent_regvote1999 ~ landless_gap +
   below1pa_gap + sharecrop_gap + service_gap + fixmoney_gap +
   fixprod_gap + per_without_instcredit + totoalkilled_1000 +
   hdi_gap1 + ln_pop2001 + totalcontestants1999 + cast_eth_fract,
   data = nepaldem, method1 = "OLS", method2 = "RLM-MM")
   
obj_cvdm_jm

model_1999 <- rlm(percent_regvote1999 ~ landless_gap +
   below1pa_gap + sharecrop_gap + service_gap + fixmoney_gap +
   fixprod_gap + per_without_instcredit + totoalkilled_1000 +
   hdi_gap1 + ln_pop2001 + totalcontestants1999 + cast_eth_fract,
   data = nepaldem)

model_1999
```

Next, we can look at a study by Golder (2010, Comparative Political Studies 43(1): 3-32). Golder employs the PLM method of estimating a Cox model to investigate Western European government formation duration. She hypothesizes that bargaining complexity leads to increasing delays in government formation as uncertainty increases. The CVMF test indicates that her choice of PLM is the better performing estimator (p < .05) compared to IRR. These data are available on [Dr. Golder's Harvard Dataverse page](https://dataverse.harvard.edu/dataset.xhtml?persistentId=doi:10.7910/DVN/BUWZBA) and have been included in the package for ease of replication. For full replication and discussion of the CVMF test, see Desmarais and Harden (2012, Political Analysis 20(1): 113-135).

```
library(survival)
library(coxrobust)
library(modeLLtest)

data(govtform)

obj_cvmf_golder <- cvmf(golder_surv ~ golder_x, method = "efron", data = govtform)
   
obj_cvmf_golder

golder_surv <- Surv(govtform$bargainingdays)
golder_x <- cbind(govtform$postelection, govtform$legislative_parties,
   govtform$polarization, govtform$positive_parl,
   govtform$post_legislative_parties,
   govtform$post_polariz, govtform$post_positive, govtform$continuation,
   govtform$singleparty_majority)
colnames(golder_x) <- c("govtform$postelection",
  "govtform$legislative_parties",
  "govtform$polarization", "govtform$positive_parl",
  "govtform$post_legislative_parties", "govtform$post_polariz",
  "govtform$post_positive", "govtform$continuation",
  "govtform$singleparty_majority")
govtform_plm <- coxph(golder_surv ~ golder_x, method = "efron",
   data = govtform)

govtform_plm
```

# What's Happening
Next steps for this package include adding more methods to the `cvdm()` and `cvll()` functions and optimizing functions to improve speed. Check back for details. 

# Contact
Please contact sscogin@nd.edu with questions or comments.
